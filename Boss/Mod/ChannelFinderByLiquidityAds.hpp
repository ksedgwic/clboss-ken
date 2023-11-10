#ifndef BOSS_MOD_CHANNELFINDERBYLIQUIDITYADS_HPP
#define BOSS_MOD_CHANNELFINDERBYLIQUIDITYADS_HPP

#include"Jsmn/Object.hpp"
#include"Ln/NodeId.hpp"
#include<cstddef>
#include<map>
#include<set>

namespace Boss { namespace Mod { class Rpc; }}
namespace Ev { template<typename a> class Io; }
namespace S { class Bus; }

namespace Boss { namespace Mod {

/** class Boss::Mod::ChannelFinderByLiquidityAds
 *
 * @brief Proposes nodes with liquidty ads,
 */
class ChannelFinderByLiquidityAds {
private:
	S::Bus& bus;
	Boss::Mod::Rpc* rpc;

	/* Peers we already have channels with.  */
	std::set<Ln::NodeId> channels;
	/* Ourself.  */
	Ln::NodeId self_id;

	bool running;

#if OLDCODE
	/* Used in a run.  */
	std::map<Ln::NodeId, std::size_t> payees;
	Jsmn::Object pays;
	Jsmn::Object::iterator it;
	std::size_t count;

	Ev::Io<void> extract_ads_loop();
#endif /* OLDCODE */

	void start();
	Ev::Io<void> fail_solicit(std::string const& reason);

public:
	ChannelFinderByLiquidityAds() =delete;
	ChannelFinderByLiquidityAds(ChannelFinderByLiquidityAds&&) =delete;
	ChannelFinderByLiquidityAds(ChannelFinderByLiquidityAds const&) =delete;

	explicit
	ChannelFinderByLiquidityAds( S::Bus& bus_
			       ) : bus(bus_)
				 , rpc(nullptr)
				 , running(false)
				 { start(); }
};

}}

#endif /* !defined(BOSS_MOD_CHANNELFINDERBYLIQUIDITYADS_HPP) */
