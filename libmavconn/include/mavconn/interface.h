/**
 * @brief MAVConn class interface
 * @file interface.h
 * @author Vladimir Ermakov <vooon341@gmail.com>
 *
 * @addtogroup mavconn
 * @{
 *  @brief MAVConn connection handling library
 *
 *  This lib provide simple interface to MAVLink enabled devices
 *  such as autopilots.
 */
/*
 * libmavconn
 * Copyright 2013,2014,2015,2016 Vladimir Ermakov, All rights reserved.
 *
 * This file is part of the mavros package and subject to the license terms
 * in the top-level LICENSE file of the mavros repository.
 * https://github.com/mavlink/mavros/tree/master/LICENSE.md
 */

#pragma once

#include <boost/system/system_error.hpp>

#include <deque>
#include <mutex>
#include <vector>
#include <atomic>
#include <chrono>
#include <thread>
#include <memory>
#include <sstream>
#include <cassert>
#include <stdexcept>
#include <unordered_map>
#include <map>
#include <set>
#include <mavconn/mavlink_dialect.h>
#include <dalpu/cipher.h>

namespace mavconn {
using steady_clock = std::chrono::steady_clock;
using lock_guard = std::lock_guard<std::recursive_mutex>;

//! Same as @p mavlink::common::MAV_COMPONENT::COMP_ID_UDP_BRIDGE
static constexpr auto MAV_COMP_ID_UDP_BRIDGE = 240;
static bool dalpu_poweron_flag = false;
//! Rx packer framing status. (same as @p mavlink::mavlink_framing_t)
enum class Framing : uint8_t {
	incomplete = mavlink::MAVLINK_FRAMING_INCOMPLETE,
	ok = mavlink::MAVLINK_FRAMING_OK,
	bad_crc = mavlink::MAVLINK_FRAMING_BAD_CRC,
	bad_signature = mavlink::MAVLINK_FRAMING_BAD_SIGNATURE,
};

//! MAVLink protocol version
enum class Protocol : uint8_t {
	V10 = 1,	//!< MAVLink v1.0
	V20 = 2		//!< MAVLink v2.0
};

/**
 * @brief Common exception for communication error
 */
class DeviceError : public std::runtime_error {
public:
	/**
	 * @breif Construct error.
	 */
	template <typename T>
	DeviceError(const char *module, T msg) :
		std::runtime_error(make_message(module, msg))
	{ }

	template <typename T>
	static std::string make_message(const char *module, T msg) {
		std::ostringstream ss;
		ss << "DeviceError:" << module << ":" << msg_to_string(msg);
		return ss.str();
	}

	static std::string msg_to_string(const char *description) {
		return description;
	}

	static std::string msg_to_string(int errnum) {
		return ::strerror(errnum);
	}

	static std::string msg_to_string(boost::system::system_error &err) {
		return err.what();
	}
};

/**
 * @brief Generic mavlink interface
 */
class MAVConnInterface {
public:
	//neowine
	void encrypt_and_crcupdate(mavlink::mavlink_message_t *msg);
	void decrypt_and_crcupdate(mavlink::mavlink_message_t *msg);
	void encrypt_and_crcupdate2(mavlink::mavlink_message_t *msg);
	void decrypt_and_crcupdate2(mavlink::mavlink_message_t *msg);

private:
	MAVConnInterface(const MAVConnInterface&) = delete;
	std::map< uint16_t, uint8_t > crc_map;
	std::set<int> rx_cx_set, rx_px_set;
public:
	using ReceivedCb = std::function<void (const mavlink::mavlink_message_t *message, const Framing framing)>;
	using ClosedCb = std::function<void (void)>;
	using Ptr = std::shared_ptr<MAVConnInterface>;
	using ConstPtr = std::shared_ptr<MAVConnInterface const>;
	using WeakPtr = std::weak_ptr<MAVConnInterface>;

	struct IOStat {
		size_t tx_total_bytes;	//!< total bytes transferred
		size_t rx_total_bytes;	//!< total bytes received
		float tx_speed;		//!< current transfer speed [B/s]
		float rx_speed;		//!< current receive speed [B/s]
	};

	/**
	 * @param[in] system_id     sysid for send_message
	 * @param[in] component_id  compid for send_message
	 */
	MAVConnInterface(uint8_t system_id = 1, uint8_t component_id = MAV_COMP_ID_UDP_BRIDGE);

	/**
	 * @brief Close connection.
	 */
	virtual void close() = 0;

	/**
	 * @brief Send message (mavlink_message_t)
	 *
	 * Can be used to forward messages from other connection channel.
	 *
	 * @note Does not do finalization!
	 *
	 * @throws std::length_error  On exceeding Tx queue limit (MAX_TXQ_SIZE)
	 * @param[in] *message  not changed
	 */
	virtual void send_message(const mavlink::mavlink_message_t *message) = 0;

	/**
	 * @brief Send message (child of mavlink::Message)
	 *
	 * Does serialization inside.
	 * System and Component ID = from this object.
	 *
	 * @throws std::length_error  On exceeding Tx queue limit (MAX_TXQ_SIZE)
	 * @param[in] &message  not changed
	 */
	virtual void send_message(const mavlink::Message &message) {
		send_message(message, this->comp_id);
	}

	/**
	 * @brief Send message (child of mavlink::Message)
	 *
	 * Does serialization inside.
	 * System ID = from this object.
	 * Component ID passed by argument.
	 *
	 * @throws std::length_error  On exceeding Tx queue limit (MAX_TXQ_SIZE)
	 * @param[in] &message  not changed
	 * @param[in] src_compid  sets the component ID of the message source
	 */
	virtual void send_message(const mavlink::Message &message, const uint8_t src_compid) = 0;

	/**
	 * @brief Send raw bytes (for some quirks)
	 * @throws std::length_error  On exceeding Tx queue limit (MAX_TXQ_SIZE)
	 */
	virtual void send_bytes(const uint8_t *bytes, size_t length) = 0;

	/**
	 * @brief Send message and ignore possible drop due to Tx queue limit
	 */
	void send_message_ignore_drop(const mavlink::mavlink_message_t *message);

	/**
	 * @brief Send message and ignore possible drop due to Tx queue limit
	 *
	 * System and Component ID = from this object.
	 */
	void send_message_ignore_drop(const mavlink::Message &message) {
		send_message_ignore_drop(message, this->comp_id);
	}

	/**
	 * @brief Send message and ignore possible drop due to Tx queue limit
	 *
	 * System ID = from this object.
	 * Component ID passed by argument.
	 */
	void send_message_ignore_drop(const mavlink::Message &message, const uint8_t src_compid);

	//! Message receive callback
	ReceivedCb message_received_cb;
	//! Port closed notification callback
	ClosedCb port_closed_cb;

	virtual mavlink::mavlink_status_t get_status();
	virtual IOStat get_iostat();
	virtual bool is_open() = 0;

	inline uint8_t get_system_id() {
		return sys_id;
	}
	inline void set_system_id(uint8_t sysid) {
		sys_id = sysid;
	}
	inline uint8_t get_component_id() {
		return comp_id;
	}
	inline void set_component_id(uint8_t compid) {
		comp_id = compid;
	}

	/**
	 * Set protocol used for encoding mavlink::Mavlink messages.
	 */
	void set_protocol_version(Protocol pver);
	Protocol get_protocol_version();

	/**
	 * @brief Construct connection from URL
	 *
	 * Supported URL schemas:
	 * - serial://
	 * - udp://
	 * - tcp://
	 * - tcp-l://
	 *
	 * Please see user's documentation for details.
	 *
	 * @param[in] url           resource locator
	 * @param[in] system_id     optional system id
	 * @param[in] component_id  optional component id
	 * @return @a Ptr to constructed interface class,
	 *         or throw @a DeviceError if error occured.
	 */
	static Ptr open_url(std::string url,
			uint8_t system_id = 1, uint8_t component_id = MAV_COMP_ID_UDP_BRIDGE);

	static std::vector<std::string> get_known_dialects();

	//static constexpr char *key_hex = "31313131313131313131313131313131";
	//static constexpr uint8_t key_hex[32] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f};
    static constexpr char *iv_hex =  "31313131313131313131313131313131";
	//static constexpr char *pt_hex = "31313131313131313131313131313131313131313131313131313131313131313232323232323232323232323232323233333333333333333333333333333333";
    //static constexpr char *ct_hex = "AD0110483D0559D12E8CB97203E6C908AD0110483D0559D12E8CB97203E6C9083288A984DD06FF58E93960B1DEC50F1E1B50B29FCA54A8D0B35D1179DC25ABC3";
	uint8_t key_hex[32];
protected:
	uint8_t sys_id;		//!< Connection System Id
	uint8_t comp_id;	//!< Connection Component Id
	uint8_t communication_type; // serial : 0, udp : 1, 

	//! Maximum mavlink packet size + some extra bytes for padding.
	static constexpr size_t MAX_PACKET_SIZE = MAVLINK_MAX_PACKET_LEN + 16;
	//! Maximum count of transmission buffers.
	static constexpr size_t MAX_TXQ_SIZE = 1000;

	//! This map merge all dialect mavlink_msg_entry_t structs. Needed for packet parser.
	static std::unordered_map<mavlink::msgid_t, const mavlink::mavlink_msg_entry_t*> message_entries;

	//! Channel number used for logging.
	size_t conn_id;

	inline mavlink::mavlink_status_t *get_status_p() {
		return &m_parse_status;
	}

	inline mavlink::mavlink_message_t *get_buffer_p() {
		return &m_buffer;
	}

	/**
	 * Parse buffer and emit massage_received.
	 */
	void parse_buffer(const char *pfx, uint8_t *buf, const size_t bufsize, size_t bytes_received);

	
	void iostat_tx_add(size_t bytes);
	void iostat_rx_add(size_t bytes);

	void log_recv(const char *pfx, mavlink::mavlink_message_t &msg, Framing framing);
	void log_send(const char *pfx, const mavlink::mavlink_message_t *msg);
	void log_send_obj(const char *pfx, const mavlink::Message &msg);

	//neowine CRC check
	uint16_t crc_calculate(const uint8_t* pBuffer, uint16_t length);
	void crc_accumulate_buffer(uint16_t *crcAccum, const char *pBuffer, uint16_t length);
	void crc_accumulate(uint8_t data, uint16_t *crcAccum);
	void crc_init(uint16_t* crcAccum);
	void crcupdate(mavlink::mavlink_message_t* msg);

private:
	friend const mavlink::mavlink_msg_entry_t* mavlink::mavlink_get_msg_entry(uint32_t msgid);

	mavlink::mavlink_status_t m_parse_status;
	mavlink::mavlink_message_t m_buffer;
	mavlink::mavlink_status_t m_mavlink_status;

	std::atomic<size_t> tx_total_bytes, rx_total_bytes;
	std::recursive_mutex iostat_mutex;
	size_t last_tx_total_bytes, last_rx_total_bytes;
	std::chrono::time_point<steady_clock> last_iostat;

	//! monotonic counter (increment only)
	static std::atomic<size_t> conn_id_counter;

	//! init_msg_entry() once flag
	static std::once_flag init_flag;

	/**
	 * Initialize message_entries map
	 *
	 * autogenerated. placed in mavlink_helpers.cpp
	 */
	static void init_msg_entry();
	int cipher_aes(dalpu_cipher_mode_t mode, dalpu_operation_t encdec, char *key_hex, char *iv_hex, char *in_hex, uint8_t* in, uint32_t in_len, uint8_t* out);
	void byte2hex(const uint8_t *buf, int32_t buf_len, char *hex);
	void hex2byte(const char *in, uint8_t *out);
};

class DalpuPayload {
    public:
        DalpuPayload(uint64_t* msg, uint8_t length);
        static const uint8_t BIT128_TO_BYTES = 16;
        static const uint32_t MAX_SIZE = 1024;
    public: 
      uint8_t input[MAX_SIZE];
	  uint8_t output[MAX_SIZE];
      uint8_t chunkCount;
      uint8_t chunkTotalBytes;
    public:
      uint8_t getChunkNum() const { return chunkCount; }
      uint8_t getTotalChunkBytes() const { return chunkTotalBytes; }
      void copyChunkBytes(uint8_t output[MAX_SIZE]) {
          memcpy(output, input, chunkTotalBytes);
      }
      void updateMessage(uint64_t* msg);

      bool compare_uint8(uint8_t* a, uint8_t* b, uint8_t size);
      bool compare_uint64(uint64_t* a, uint64_t* b, uint8_t size);
      void print_status() { std::cout<< "chunk Count : "<<(uint32_t)chunkCount << " Total Bytes of Chunk : "<< (uint32_t)chunkTotalBytes << std::endl; }
      void print_input() { 
          for(int i=0; i<MAX_SIZE; i++) {
              std::cout<< (uint32_t)input[i] << " ";
          }
          std::cout<<std::endl;
      }

};
}	// namespace mavconn
