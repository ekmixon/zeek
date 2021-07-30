#pragma once

#include "zeek/analyzer/protocol/ssl/events.bif.h"

namespace binpac
	{
namespace DTLS
	{
class SSL_Conn;
	}
	}
namespace binpac
	{
namespace TLSHandshake
	{
class Handshake_Conn;
	}
	}

namespace zeek::analyzer::dtls
	{

class DTLS_Analyzer final : public analyzer::Analyzer
	{
public:
	explicit DTLS_Analyzer(Connection* conn);
	~DTLS_Analyzer() override;

	// Overriden from Analyzer.
	void Done() override;
	void DeliverPacket(int len, const u_char* data, bool orig, uint64_t seq, const IP_Hdr* ip,
	                   int caplen) override;
	void EndOfData(bool is_orig) override;

	void SendHandshake(uint16_t raw_tls_version, uint8_t msg_type, uint32_t length,
	                   const u_char* begin, const u_char* end, bool orig);
	// Get the TLS version that the server chose. 0 if not yet known.
	uint16_t GetNegotiatedVersion() const;

	static analyzer::Analyzer* Instantiate(Connection* conn) { return new DTLS_Analyzer(conn); }

protected:
	binpac::DTLS::SSL_Conn* interp;
	binpac::TLSHandshake::Handshake_Conn* handshake_interp;
	};

	} // namespace zeek::analyzer::dtls
