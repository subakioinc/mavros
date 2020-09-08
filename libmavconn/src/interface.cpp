/**
 * @brief MAVConn class interface
 * @file interface.cpp
 * @author Vladimir Ermakov <vooon341@gmail.com>
 *
 * @addtogroup mavconn
 * @{
 */
/*
 * libmavconn
 * Copyright 2013,2014,2015,2016 Vladimir Ermakov, All rights reserved.
 *
 * This file is part of the mavros package and subject to the license terms
 * in the top-level LICENSE file of the mavros repository.
 * https://github.com/mavlink/mavros/tree/master/LICENSE.md
 */

#include <set>
#include <cassert>

#include <mavconn/console_bridge_compat.h>
#include <mavconn/interface.h>
#include <mavconn/msgbuffer.h>
#include <mavconn/serial.h>
#include <mavconn/udp.h>
#include <mavconn/tcp.h>
#include <dalpu/aes.h>

namespace mavconn {
#define PFX	"mavconn: "

using mavlink::mavlink_message_t;
using mavlink::mavlink_status_t;

// static members
std::once_flag MAVConnInterface::init_flag;
std::unordered_map<mavlink::msgid_t, const mavlink::mavlink_msg_entry_t*> MAVConnInterface::message_entries {};
std::atomic<size_t> MAVConnInterface::conn_id_counter {0};


MAVConnInterface::MAVConnInterface(uint8_t system_id, uint8_t component_id) :
	sys_id(system_id),
	comp_id(component_id),
	m_parse_status {},
	m_buffer {},
	m_mavlink_status {},
	tx_total_bytes(0),
	rx_total_bytes(0),
	last_tx_total_bytes(0),
	last_rx_total_bytes(0),
	last_iostat(steady_clock::now())
{
	conn_id = conn_id_counter.fetch_add(1);
	std::call_once(init_flag, init_msg_entry);
	using namespace std;
	crc_map.insert(make_pair( 0  , 50 ));
	crc_map.insert(make_pair( 1  , 124 ));
	crc_map.insert(make_pair( 2  , 137 ));
	crc_map.insert(make_pair( 4  , 237 ));
	crc_map.insert(make_pair( 5  , 217 ));
	crc_map.insert(make_pair( 6  , 104 ));
	crc_map.insert(make_pair( 7  , 119 ));
	crc_map.insert(make_pair( 8  , 117 ));
	crc_map.insert(make_pair( 11  , 89 ));
	crc_map.insert(make_pair( 19  , 137 ));
	crc_map.insert(make_pair( 20  , 214 ));
	crc_map.insert(make_pair( 21  , 159 ));
	crc_map.insert(make_pair( 22  , 220 ));
	crc_map.insert(make_pair( 23  , 168 ));
	crc_map.insert(make_pair( 24  , 24 ));
	crc_map.insert(make_pair( 25  , 23 ));
	crc_map.insert(make_pair( 26  , 170 ));
	crc_map.insert(make_pair( 27  , 144 ));
	crc_map.insert(make_pair( 28  , 67 ));
	crc_map.insert(make_pair( 29  , 115 ));
	crc_map.insert(make_pair( 30  , 39 ));
	crc_map.insert(make_pair( 31  , 246 ));
	crc_map.insert(make_pair( 32  , 185 ));
	crc_map.insert(make_pair( 33  , 104 ));
	crc_map.insert(make_pair( 34  , 237 ));
	crc_map.insert(make_pair( 35  , 244 ));
	crc_map.insert(make_pair( 36  , 222 ));
	crc_map.insert(make_pair( 37  , 212 ));
	crc_map.insert(make_pair( 38  , 9 ));
	crc_map.insert(make_pair( 39  , 254 ));
	crc_map.insert(make_pair( 40  , 230 ));
	crc_map.insert(make_pair( 41  , 28 ));
	crc_map.insert(make_pair( 42  , 28 ));
	crc_map.insert(make_pair( 43  , 132 ));
	crc_map.insert(make_pair( 44  , 221 ));
	crc_map.insert(make_pair( 45  , 232 ));
	crc_map.insert(make_pair( 46  , 11 ));
	crc_map.insert(make_pair( 47  , 153 ));
	crc_map.insert(make_pair( 48  , 41 ));
	crc_map.insert(make_pair( 49  , 39 ));
	crc_map.insert(make_pair( 50  , 78 ));
	crc_map.insert(make_pair( 51  , 196 ));
	crc_map.insert(make_pair( 52  , 132 ));
	crc_map.insert(make_pair( 54  , 15 ));
	crc_map.insert(make_pair( 55  , 3 ));
	crc_map.insert(make_pair( 61  , 167 ));
	crc_map.insert(make_pair( 62  , 183 ));
	crc_map.insert(make_pair( 63  , 119 ));
	crc_map.insert(make_pair( 64  , 191 ));
	crc_map.insert(make_pair( 65  , 118 ));
	crc_map.insert(make_pair( 66  , 148 ));
	crc_map.insert(make_pair( 67  , 21 ));
	crc_map.insert(make_pair( 69  , 243 ));
	crc_map.insert(make_pair( 70  , 124 ));
	crc_map.insert(make_pair( 73  , 38 ));
	crc_map.insert(make_pair( 74  , 20 ));
	crc_map.insert(make_pair( 75  , 158 ));
	crc_map.insert(make_pair( 76  , 152 ));
	crc_map.insert(make_pair( 77  , 143 ));
	crc_map.insert(make_pair( 80  , 14 ));
	crc_map.insert(make_pair( 81  , 106 ));
	crc_map.insert(make_pair( 82  , 49 ));
	crc_map.insert(make_pair( 83  , 22 ));
	crc_map.insert(make_pair( 84  , 143 ));
	crc_map.insert(make_pair( 85  , 140 ));
	crc_map.insert(make_pair( 86  , 5 ));
	crc_map.insert(make_pair( 87  , 150 ));
	crc_map.insert(make_pair( 89  , 231 ));
	crc_map.insert(make_pair( 90  , 183 ));
	crc_map.insert(make_pair( 91  , 63 ));
	crc_map.insert(make_pair( 92  , 54 ));
	crc_map.insert(make_pair( 93  , 47 ));
	crc_map.insert(make_pair( 100  , 175 ));
	crc_map.insert(make_pair( 101  , 102 ));
	crc_map.insert(make_pair( 102  , 158 ));
	crc_map.insert(make_pair( 103  , 208 ));
	crc_map.insert(make_pair( 104  , 56 ));
	crc_map.insert(make_pair( 105  , 93 ));
	crc_map.insert(make_pair( 106  , 138 ));
	crc_map.insert(make_pair( 107  , 108 ));
	crc_map.insert(make_pair( 108  , 32 ));
	crc_map.insert(make_pair( 109  , 185 ));
	crc_map.insert(make_pair( 110  , 84 ));
	crc_map.insert(make_pair( 111  , 34 ));
	crc_map.insert(make_pair( 112  , 174 ));
	crc_map.insert(make_pair( 113  , 124 ));
	crc_map.insert(make_pair( 114  , 237 ));
	crc_map.insert(make_pair( 115  , 4 ));
	crc_map.insert(make_pair( 116  , 76 ));
	crc_map.insert(make_pair( 117  , 128 ));
	crc_map.insert(make_pair( 118  , 56 ));
	crc_map.insert(make_pair( 119  , 116 ));
	crc_map.insert(make_pair( 120  , 134 ));
	crc_map.insert(make_pair( 121  , 237 ));
	crc_map.insert(make_pair( 122  , 203 ));
	crc_map.insert(make_pair( 123  , 250 ));
	crc_map.insert(make_pair( 124  , 87 ));
	crc_map.insert(make_pair( 125  , 203 ));
	crc_map.insert(make_pair( 126  , 220 ));
	crc_map.insert(make_pair( 127  , 25 ));
	crc_map.insert(make_pair( 128  , 226 ));
	crc_map.insert(make_pair( 129  , 46 ));
	crc_map.insert(make_pair( 130  , 29 ));
	crc_map.insert(make_pair( 131  , 223 ));
	crc_map.insert(make_pair( 132  , 85 ));
	crc_map.insert(make_pair( 133  , 6 ));
	crc_map.insert(make_pair( 134  , 229 ));
	crc_map.insert(make_pair( 135  , 203 ));
	crc_map.insert(make_pair( 136  , 1 ));
	crc_map.insert(make_pair( 137  , 195 ));
	crc_map.insert(make_pair( 138  , 109 ));
	crc_map.insert(make_pair( 139  , 168 ));
	crc_map.insert(make_pair( 140  , 181 ));
	crc_map.insert(make_pair( 141  , 47 ));
	crc_map.insert(make_pair( 142  , 72 ));
	crc_map.insert(make_pair( 143  , 131 ));
	crc_map.insert(make_pair( 144  , 127 ));
	crc_map.insert(make_pair( 146  , 103 ));
	crc_map.insert(make_pair( 147  , 154 ));
	crc_map.insert(make_pair( 148  , 178 ));
	crc_map.insert(make_pair( 149  , 200 ));
	crc_map.insert(make_pair( 162  , 189 ));
	crc_map.insert(make_pair( 230  , 163 ));
	crc_map.insert(make_pair( 231  , 105 ));
	crc_map.insert(make_pair( 232  , 151 ));
	crc_map.insert(make_pair( 233  , 35 ));
	crc_map.insert(make_pair( 234  , 150 ));
	crc_map.insert(make_pair( 235  , 179 ));
	crc_map.insert(make_pair( 241  , 90 ));
	crc_map.insert(make_pair( 242  , 104 ));
	crc_map.insert(make_pair( 243  , 85 ));
	crc_map.insert(make_pair( 244  , 95 ));
	crc_map.insert(make_pair( 245  , 130 ));
	crc_map.insert(make_pair( 246  , 184 ));
	crc_map.insert(make_pair( 247  , 81 ));
	crc_map.insert(make_pair( 248  , 8 ));
	crc_map.insert(make_pair( 249  , 204 ));
	crc_map.insert(make_pair( 250  , 49 ));
	crc_map.insert(make_pair( 251  , 170 ));
	crc_map.insert(make_pair( 252  , 44 ));
	crc_map.insert(make_pair( 253  , 83 ));
	crc_map.insert(make_pair( 254  , 46 ));
	crc_map.insert(make_pair( 256  , 71 ));
	crc_map.insert(make_pair( 257  , 131 ));
	crc_map.insert(make_pair( 258  , 187 ));
	crc_map.insert(make_pair( 259  , 92 ));
	crc_map.insert(make_pair( 260  , 146 ));
	crc_map.insert(make_pair( 261  , 179 ));
	crc_map.insert(make_pair( 262  , 12 ));
	crc_map.insert(make_pair( 263  , 133 ));
	crc_map.insert(make_pair( 264  , 49 ));
	crc_map.insert(make_pair( 265  , 26 ));
	crc_map.insert(make_pair( 266  , 193 ));
	crc_map.insert(make_pair( 267  , 35 ));
	crc_map.insert(make_pair( 268  , 14 ));
	crc_map.insert(make_pair( 269  , 109 ));
	crc_map.insert(make_pair( 270  , 59 ));
	crc_map.insert(make_pair( 271  , 22 ));
	crc_map.insert(make_pair( 275  , 126 ));
	crc_map.insert(make_pair( 276  , 18 ));
	crc_map.insert(make_pair( 280  , 166 ));
	crc_map.insert(make_pair( 281  , 0 ));
	crc_map.insert(make_pair( 282  , 123 ));
	crc_map.insert(make_pair( 283  , 247 ));
	crc_map.insert(make_pair( 284  , 99 ));
	crc_map.insert(make_pair( 285  , 137 ));
	crc_map.insert(make_pair( 286  , 210 ));
	crc_map.insert(make_pair( 287  , 74 ));
	crc_map.insert(make_pair( 290  , 221 ));
	crc_map.insert(make_pair( 291  , 10 ));
	crc_map.insert(make_pair( 299  , 19 ));
	crc_map.insert(make_pair( 300  , 217 ));
	crc_map.insert(make_pair( 301  , 243 ));
	crc_map.insert(make_pair( 310  , 28 ));
	crc_map.insert(make_pair( 311  , 95 ));
	crc_map.insert(make_pair( 320  , 243 ));
	crc_map.insert(make_pair( 321  , 88 ));
	crc_map.insert(make_pair( 322  , 243 ));
	crc_map.insert(make_pair( 323  , 78 ));
	crc_map.insert(make_pair( 324  , 132 ));
	crc_map.insert(make_pair( 325  , 228 ));
	crc_map.insert(make_pair( 326  , 24 ));
	crc_map.insert(make_pair( 327  , 178 ));
	crc_map.insert(make_pair( 328  , 160 ));
	crc_map.insert(make_pair( 329  , 121 ));
	crc_map.insert(make_pair( 330  , 23 ));
	crc_map.insert(make_pair( 331  , 91 ));
	crc_map.insert(make_pair( 332  , 236 ));
	crc_map.insert(make_pair( 333  , 231 ));
	crc_map.insert(make_pair( 334  , 72 ));
	crc_map.insert(make_pair( 335  , 225 ));
	crc_map.insert(make_pair( 336  , 245 ));
	crc_map.insert(make_pair( 339  , 199 ));
	crc_map.insert(make_pair( 340  , 99 ));
	crc_map.insert(make_pair( 350  , 232 ));
	crc_map.insert(make_pair( 360  , 11 ));
	crc_map.insert(make_pair( 370  , 98 ));
	crc_map.insert(make_pair( 371  , 161 ));
	crc_map.insert(make_pair( 373  , 117 ));
	crc_map.insert(make_pair( 375  , 251 ));
	crc_map.insert(make_pair( 380  , 232 ));
	crc_map.insert(make_pair( 385  , 147 ));
	crc_map.insert(make_pair( 390  , 156 ));
	crc_map.insert(make_pair( 395  , 163 ));
	crc_map.insert(make_pair( 400  , 110 ));
	crc_map.insert(make_pair( 401  , 183 ));
	crc_map.insert(make_pair( 9000  , 113 ));
	crc_map.insert(make_pair( 9005  , 117 ));
	crc_map.insert(make_pair( 12900  , 114 ));
	crc_map.insert(make_pair( 12901  , 254 ));
	crc_map.insert(make_pair( 12902  , 49 ));
	crc_map.insert(make_pair( 12903  , 249 ));
	crc_map.insert(make_pair( 12904  , 203 ));
	crc_map.insert(make_pair( 12905  , 49 ));
	crc_map.insert(make_pair( 12915  , 62 ));
	crc_map.insert(make_pair( 10001  , 209 ));
	crc_map.insert(make_pair( 10002  , 186 ));
	crc_map.insert(make_pair( 10003  , 4 ));
	crc_map.insert(make_pair( 42000  , 227 ));
	crc_map.insert(make_pair( 42001  , 239 ));
}

mavlink_status_t MAVConnInterface::get_status()
{
	return m_mavlink_status;
}

MAVConnInterface::IOStat MAVConnInterface::get_iostat()
{
	std::lock_guard<std::recursive_mutex> lock(iostat_mutex);
	IOStat stat;

	stat.tx_total_bytes = tx_total_bytes;
	stat.rx_total_bytes = rx_total_bytes;

	auto d_tx = stat.tx_total_bytes - last_tx_total_bytes;
	auto d_rx = stat.rx_total_bytes - last_rx_total_bytes;
	last_tx_total_bytes = stat.tx_total_bytes;
	last_rx_total_bytes = stat.rx_total_bytes;

	auto now = steady_clock::now();
	auto dt = now - last_iostat;
	last_iostat = now;

	float dt_s = std::chrono::duration_cast<std::chrono::seconds>(dt).count();

	stat.tx_speed = d_tx / dt_s;
	stat.rx_speed = d_rx / dt_s;

	return stat;
}

void MAVConnInterface::iostat_tx_add(size_t bytes)
{
	tx_total_bytes += bytes;
}

void MAVConnInterface::iostat_rx_add(size_t bytes)
{
	rx_total_bytes += bytes;
}

void MAVConnInterface::parse_buffer(const char *pfx, uint8_t *buf, const size_t bufsize, size_t bytes_received)
{
	mavlink::mavlink_message_t message;

	assert(bufsize >= bytes_received);

	iostat_rx_add(bytes_received);
	for (; bytes_received > 0; bytes_received--) {
		auto c = *buf++;

		auto msg_received = static_cast<Framing>(mavlink::mavlink_frame_char_buffer(&m_buffer, &m_parse_status, c, &message, &m_mavlink_status));

		if (msg_received != Framing::incomplete) {
			log_recv(pfx, message, msg_received);

			if (message_received_cb)
			{
				if(communication_type ==0) { //serial (PX4 -> mavros)
					encrypt_and_crcupdate(&message);
				} else if (communication_type = 1) { // UDP (QGC -> mavros)
					decrypt_and_crcupdate(&message);
				} else {  // TCP nothing to do 

				}
				message_received_cb(&message, msg_received);
			}
		}
	}
}


void MAVConnInterface::encrypt_and_crcupdate(mavlink::mavlink_message_t *msg)
{
	//printf("처음 : msg->payload64, msg->crc : %lld , %d \n", msg->payload64[0], msg->checksum);
	//uint64_t check_value = msg->payload64[0] & 0xff;
	//printf("메시지 초기값 : %lld", check_value);
	uint16_t init_checksum = msg->checksum;
	uint64_t firstItem = msg->payload64[0];
	firstItem = firstItem ^ 0xff;
	msg->payload64[0] = firstItem;
	
	crcupdate(msg);
}

void MAVConnInterface::decrypt_and_crcupdate(mavlink::mavlink_message_t *msg)
{
	//printf("처음 : msg->payload64, msg->crc : %lld , %d \n", msg->payload64[0], msg->checksum);
	//uint64_t check_value = msg->payload64[0] & 0xff;
	//printf("메시지 초기값 : %lld", check_value);
	uint16_t init_checksum = msg->checksum;
	uint64_t firstItem = msg->payload64[0];
	firstItem = firstItem ^ 0xff;
	msg->payload64[0] = firstItem;
	
	crcupdate(msg);
}


uint16_t MAVConnInterface::crc_calculate(const uint8_t* pBuffer, uint16_t length)
{
        uint16_t crcTmp;
        crc_init(&crcTmp);
		while (length--) {
                crc_accumulate(*pBuffer++, &crcTmp);
        }
        return crcTmp;
}

void MAVConnInterface::crc_accumulate_buffer(uint16_t *crcAccum, const char *pBuffer, uint16_t length)
{
	const uint8_t *p = (const uint8_t *)pBuffer;
	while (length--) {
                crc_accumulate(*p++, crcAccum);
        }
}

void MAVConnInterface::crc_accumulate(uint8_t data, uint16_t *crcAccum)
{
	/*Accumulate one byte of data into the CRC*/
        uint8_t tmp;

        tmp = data ^ (uint8_t)(*crcAccum &0xff);
        tmp ^= (tmp<<4);
        *crcAccum = (*crcAccum>>8) ^ (tmp<<8) ^ (tmp <<3) ^ (tmp>>4);
}

void MAVConnInterface::crc_init(uint16_t* crcAccum)
{
		#define X25_INIT_CRC 0xffff
        *crcAccum = X25_INIT_CRC;
}
void MAVConnInterface::crcupdate(mavlink_message_t* msg)
{
	#define _MAV_PAYLOAD(msg) ((const char *)(&((msg)->payload64[0])))
    #define _MAV_PAYLOAD_NON_CONST(msg) ((char *)(&((msg)->payload64[0])))

    #define mavlink_ck_a(msg) *((msg)->len + (uint8_t *)_MAV_PAYLOAD_NON_CONST(msg))
    #define mavlink_ck_b(msg) *(((msg)->len+(uint16_t)1) + (uint8_t *)_MAV_PAYLOAD_NON_CONST(msg))

    #define MAVLINK_STX_MAVLINK1 0xFE

    uint8_t header_len = MAVLINK_CORE_HEADER_LEN+1;
    uint8_t buf[MAVLINK_CORE_HEADER_LEN+1];

    bool mavlink1 = false;
    if (msg->magic == MAVLINK_STX_MAVLINK1) 
    {
        mavlink1 = true;
    }
    
    // form the header as a byte array for the crc
	buf[0] = msg->magic;
	buf[1] = msg->len;
	if (mavlink1) {
		buf[2] = msg->seq;
		buf[3] = msg->sysid;
		buf[4] = msg->compid;
		buf[5] = msg->msgid & 0xFF;
	} else {
		buf[2] = msg->incompat_flags;
		buf[3] = msg->compat_flags;
		buf[4] = msg->seq;
		buf[5] = msg->sysid;
		buf[6] = msg->compid;
		buf[7] = msg->msgid & 0xFF;
		buf[8] = (msg->msgid >> 8) & 0xFF;
		buf[9] = (msg->msgid >> 16) & 0xFF;
	}
	
	uint16_t checksum = crc_calculate(&buf[1], header_len-1);
	crc_accumulate_buffer(&checksum, _MAV_PAYLOAD(msg), msg->len);
 	//uint8_t crc_extra = msg->msgid >= crcs.size() ? 0 : crcs.at(msg->msgid) ;
	uint8_t crc_extra = 0;
	auto it = crc_map.find(msg->msgid);
	if(it != crc_map.end()) {
		crc_extra = it->second;
	}
	crc_accumulate(crc_extra, &checksum);
	mavlink_ck_a(msg) = (uint8_t)(checksum & 0xFF);
	mavlink_ck_b(msg) = (uint8_t)(checksum >> 8);

	msg->checksum = checksum;
}

void MAVConnInterface::log_recv(const char *pfx, mavlink_message_t &msg, Framing framing)
{
	const char *framing_str = (framing == Framing::ok) ? "OK" :
			(framing == Framing::bad_crc) ? "!CRC" :
			(framing == Framing::bad_signature) ? "!SIG" : "ERR";

	const char *proto_version_str = (msg.magic == MAVLINK_STX) ? "v2.0" : "v1.0";

	CONSOLE_BRIDGE_logDebug("%s%zu: recv: %s %4s Message-Id: %u [%u bytes] IDs: %u.%u Seq: %u",
			pfx, conn_id,
			proto_version_str,
			framing_str,
			msg.msgid, msg.len, msg.sysid, msg.compid, msg.seq);
}

void MAVConnInterface::log_send(const char *pfx, const mavlink_message_t *msg)
{
	const char *proto_version_str = (msg->magic == MAVLINK_STX) ? "v2.0" : "v1.0";

	CONSOLE_BRIDGE_logDebug("%s%zu: send: %s Message-Id: %u [%u bytes] IDs: %u.%u Seq: %u",
			pfx, conn_id,
			proto_version_str,
			msg->msgid, msg->len, msg->sysid, msg->compid, msg->seq);
}

void MAVConnInterface::log_send_obj(const char *pfx, const mavlink::Message &msg)
{
	CONSOLE_BRIDGE_logDebug("%s%zu: send: %s", pfx, conn_id, msg.to_yaml().c_str());
}

void MAVConnInterface::send_message_ignore_drop(const mavlink::mavlink_message_t *msg)
{
	try {
		send_message(msg);
	}
	catch (std::length_error &e) {
		CONSOLE_BRIDGE_logError(PFX "%zu: DROPPED Message-Id %u [%u bytes] IDs: %u.%u Seq: %u: %s",
				conn_id,
				msg->msgid, msg->len, msg->sysid, msg->compid, msg->seq,
				e.what());
	}
}

void MAVConnInterface::send_message_ignore_drop(const mavlink::Message &msg, uint8_t source_compid)
{
	try {
		send_message(msg, source_compid);
	}
	catch (std::length_error &e) {
		CONSOLE_BRIDGE_logError(PFX "%zu: DROPPED Message %s: %s",
				conn_id,
				msg.get_name().c_str(),
				e.what());
	}
}

void MAVConnInterface::set_protocol_version(Protocol pver)
{
	if (pver == Protocol::V10)
		m_mavlink_status.flags |= MAVLINK_STATUS_FLAG_OUT_MAVLINK1;
	else
		m_mavlink_status.flags &= ~(MAVLINK_STATUS_FLAG_OUT_MAVLINK1);
}

Protocol MAVConnInterface::get_protocol_version()
{
	if (m_mavlink_status.flags & MAVLINK_STATUS_FLAG_OUT_MAVLINK1)
		return Protocol::V10;
	else
		return Protocol::V20;
}

/**
 * Parse host:port pairs
 */
static void url_parse_host(std::string host,
		std::string &host_out, int &port_out,
		const std::string def_host, const int def_port)
{
	std::string port;

	auto sep_it = std::find(host.begin(), host.end(), ':');
	if (sep_it == host.end()) {
		// host
		if (!host.empty()) {
			host_out = host;
			port_out = def_port;
		}
		else {
			host_out = def_host;
			port_out = def_port;
		}
		return;
	}

	if (sep_it == host.begin()) {
		// :port
		host_out = def_host;
	}
	else {
		// host:port
		host_out.assign(host.begin(), sep_it);
	}

	port.assign(sep_it + 1, host.end());
	port_out = std::stoi(port);
}

/**
 * Parse ?ids=sid,cid
 */
static void url_parse_query(std::string query, uint8_t &sysid, uint8_t &compid)
{
	const std::string ids_end("ids=");
	std::string sys, comp;

	if (query.empty())
		return;

	auto ids_it = std::search(query.begin(), query.end(),
			ids_end.begin(), ids_end.end());
	if (ids_it == query.end()) {
		CONSOLE_BRIDGE_logWarn(PFX "URL: unknown query arguments");
		return;
	}

	std::advance(ids_it, ids_end.length());
	auto comma_it = std::find(ids_it, query.end(), ',');
	if (comma_it == query.end()) {
		CONSOLE_BRIDGE_logError(PFX "URL: no comma in ids= query");
		return;
	}

	sys.assign(ids_it, comma_it);
	comp.assign(comma_it + 1, query.end());

	sysid = std::stoi(sys);
	compid = std::stoi(comp);

	CONSOLE_BRIDGE_logDebug(PFX "URL: found system/component id = [%u, %u]", sysid, compid);
}

static MAVConnInterface::Ptr url_parse_serial(
		std::string path, std::string query,
		uint8_t system_id, uint8_t component_id, bool hwflow)
{
	std::string file_path;
	int baudrate;

	// /dev/ttyACM0:57600
	url_parse_host(path, file_path, baudrate, MAVConnSerial::DEFAULT_DEVICE, MAVConnSerial::DEFAULT_BAUDRATE);
	url_parse_query(query, system_id, component_id);

	return std::make_shared<MAVConnSerial>(system_id, component_id,
			file_path, baudrate, hwflow);
}

static MAVConnInterface::Ptr url_parse_udp(
		std::string hosts, std::string query,
		uint8_t system_id, uint8_t component_id, bool is_udpb, bool permanent_broadcast)
{
	std::string bind_pair, remote_pair;
	std::string bind_host, remote_host;
	int bind_port, remote_port;

	auto sep_it = std::find(hosts.begin(), hosts.end(), '@');
	if (sep_it == hosts.end()) {
		CONSOLE_BRIDGE_logError(PFX "UDP URL should contain @!");
		throw DeviceError("url", "UDP separator not found");
	}

	bind_pair.assign(hosts.begin(), sep_it);
	remote_pair.assign(sep_it + 1, hosts.end());

	// udp://0.0.0.0:14555@:14550
	url_parse_host(bind_pair, bind_host, bind_port, "0.0.0.0", MAVConnUDP::DEFAULT_BIND_PORT);
	url_parse_host(remote_pair, remote_host, remote_port, MAVConnUDP::DEFAULT_REMOTE_HOST, MAVConnUDP::DEFAULT_REMOTE_PORT);
	url_parse_query(query, system_id, component_id);

	if (is_udpb)
		remote_host = permanent_broadcast ? MAVConnUDP::PERMANENT_BROADCAST_REMOTE_HOST : MAVConnUDP::BROADCAST_REMOTE_HOST;

	return std::make_shared<MAVConnUDP>(system_id, component_id,
			bind_host, bind_port,
			remote_host, remote_port);
}

static MAVConnInterface::Ptr url_parse_tcp_client(
		std::string host, std::string query,
		uint8_t system_id, uint8_t component_id)
{
	std::string server_host;
	int server_port;

	// tcp://localhost:5760
	url_parse_host(host, server_host, server_port, "localhost", 5760);
	url_parse_query(query, system_id, component_id);

	return std::make_shared<MAVConnTCPClient>(system_id, component_id,
			server_host, server_port);
}

static MAVConnInterface::Ptr url_parse_tcp_server(
		std::string host, std::string query,
		uint8_t system_id, uint8_t component_id)
{
	std::string bind_host;
	int bind_port;

	// tcp-l://0.0.0.0:5760
	url_parse_host(host, bind_host, bind_port, "0.0.0.0", 5760);
	url_parse_query(query, system_id, component_id);

	return std::make_shared<MAVConnTCPServer>(system_id, component_id,
			bind_host, bind_port);
}

MAVConnInterface::Ptr MAVConnInterface::open_url(std::string url,
		uint8_t system_id, uint8_t component_id)
{
	/* Based on code found here:
	 * http://stackoverflow.com/questions/2616011/easy-way-to-parse-a-url-in-c-cross-platform
	 */

	const std::string proto_end("://");
	std::string proto;
	std::string host;
	std::string path;
	std::string query;

	auto proto_it = std::search(
			url.begin(), url.end(),
			proto_end.begin(), proto_end.end());
	if (proto_it == url.end()) {
		// looks like file path
		CONSOLE_BRIDGE_logDebug(PFX "URL: %s: looks like file path", url.c_str());
		return url_parse_serial(url, "", system_id, component_id, false);
	}

	// copy protocol
	proto.reserve(std::distance(url.begin(), proto_it));
	std::transform(url.begin(), proto_it,
			std::back_inserter(proto),
			std::ref(tolower));

	// copy host
	std::advance(proto_it, proto_end.length());
	auto path_it = std::find(proto_it, url.end(), '/');
	std::transform(proto_it, path_it,
			std::back_inserter(host),
			std::ref(tolower));

	// copy path, and query if exists
	auto query_it = std::find(path_it, url.end(), '?');
	path.assign(path_it, query_it);
	if (query_it != url.end())
		++query_it;
	query.assign(query_it, url.end());

	CONSOLE_BRIDGE_logDebug(PFX "URL: %s: proto: %s, host: %s, path: %s, query: %s",
			url.c_str(), proto.c_str(), host.c_str(),
			path.c_str(), query.c_str());

	if (proto == "udp")
		return url_parse_udp(host, query, system_id, component_id, false, false);
	else if (proto == "udp-b")
		return url_parse_udp(host, query, system_id, component_id, true, false);
	else if (proto == "udp-pb")
		return url_parse_udp(host, query, system_id, component_id, true, true);
	else if (proto == "tcp")
		return url_parse_tcp_client(host, query, system_id, component_id);
	else if (proto == "tcp-l")
		return url_parse_tcp_server(host, query, system_id, component_id);
	else if (proto == "serial")
		return url_parse_serial(path, query, system_id, component_id, false);
	else if (proto == "serial-hwfc")
		return url_parse_serial(path, query, system_id, component_id, true);
	else
		throw DeviceError("url", "Unknown URL type");
}
}	// namespace mavconn
