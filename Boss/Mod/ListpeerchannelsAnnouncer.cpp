#include"Boss/Mod/ListpeerchannelsAnnouncer.hpp"
#include"Boss/ModG/RpcProxy.hpp"
#include"Boss/Msg/Init.hpp"
#include"Boss/Msg/ListpeerchannelsResult.hpp"
#include"Boss/Msg/Timer10Minutes.hpp"
#include"Boss/concurrent.hpp"
#include"Boss/log.hpp"
#include"Ev/Io.hpp"
#include"Jsmn/Object.hpp"
#include"Json/Out.hpp"
#include"S/Bus.hpp"
#include"Util/make_unique.hpp"
#include<assert.h>
#include<sstream>

namespace Boss { namespace Mod {

void ListpeerchannelsAnnouncer::start() {
	auto invalid_listpeerchannels = [this](Jsmn::Object result) {
		auto os = std::ostringstream();
		os << result;
		return Boss::log( bus, Error
				, "ListpeerchannelsAnnouncer: invalid result from "
				  "`listpeerchannels`."
				);
	};
	auto do_listpeerchannels = [ this
			    , invalid_listpeerchannels
			    ](bool initial) {
		return rpc->command("listpeerchannels"
				   , Json::Out::empty_object()
				   ).then([ this
					  , initial
					  , invalid_listpeerchannels
					  ](Jsmn::Object result) {
			if (!result.is_object() || !result.has("channels"))
				return invalid_listpeerchannels(std::move(result));
			auto peerchannels = result["channels"];
			if (!peerchannels.is_array())
				return invalid_listpeerchannels(std::move(result));

			return bus.raise(Msg::ListpeerchannelsResult{
				std::move(peerchannels), initial
			});
		});
	};
	bus.subscribe<Msg::Init
		     >([do_listpeerchannels](Msg::Init const& init) {
		return Boss::concurrent(do_listpeerchannels(true));
	});
	bus.subscribe<Msg::Timer10Minutes
		     >([do_listpeerchannels](Msg::Timer10Minutes const& _) {
		return Boss::concurrent(do_listpeerchannels(false));
	});
}

ListpeerchannelsAnnouncer::ListpeerchannelsAnnouncer(S::Bus& bus_)
	: bus(bus_)
	, rpc(Util::make_unique<ModG::RpcProxy>(bus))
	{ start(); }

ListpeerchannelsAnnouncer::~ListpeerchannelsAnnouncer() =default;

}}
