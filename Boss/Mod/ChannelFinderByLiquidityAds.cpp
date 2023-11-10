#include"Boss/Mod/ChannelFinderByLiquidityAds.hpp"
#include"Boss/Mod/Rpc.hpp"
#include"Boss/Msg/Init.hpp"
#include"Boss/Msg/ListpeersAnalyzedResult.hpp"
#include"Boss/Msg/ProposePatronlessChannelCandidate.hpp"
#include"Boss/Msg/SolicitChannelCandidates.hpp"
#include"Boss/concurrent.hpp"
#include"Boss/log.hpp"
#include"Boss/random_engine.hpp"
#include"Ev/Io.hpp"
#include"Ev/foreach.hpp"
#include"Ev/yield.hpp"
#include"Jsmn/Object.hpp"
#include"Json/Out.hpp"
#include"S/Bus.hpp"
#include"Stats/ReservoirSampler.hpp"
#include"Util/stringify.hpp"
#include<algorithm>
#include<iterator>
#include<deque>
#include<map>

namespace Boss { namespace Mod {

void ChannelFinderByLiquidityAds::start() {
	bus.subscribe<Msg::Init
				  >([this](Msg::Init const& init) {
					  rpc = &init.rpc;
					  self_id = init.self_id;
					  return Boss::log( bus, Info ,
										"ChannelFinderByLiquidityAds: "
										"saw Msg::Init");
					  // return Ev::lift();
				  });
	bus.subscribe<Msg::ListpeersAnalyzedResult
				  >([this](Msg::ListpeersAnalyzedResult const& r) {
					  channels.clear();
					  std::set_union( r.connected_channeled.begin()
									  , r.connected_channeled.end()
									  , r.disconnected_channeled.begin()
									  , r.disconnected_channeled.end()
									  , std::inserter(channels, channels.begin())
						  );
					  return Boss::log( bus, Info ,
										"ChannelFinderByLiquidityAds: "
										"saw Msg::ListpeersAnalyzedResult");
					  // return Ev::lift();
				  });
	bus.subscribe<Msg::SolicitChannelCandidates
				  >([this](Msg::SolicitChannelCandidates const& m) {
					  if (!rpc)
						  return Ev::lift();
					  if (running)
						  return Ev::lift();

					  running = true;

					  auto act = rpc->command( "listnodes", Json::Out::empty_object()
						  ).then([this](Jsmn::Object res) {
							  if (!res.is_object() || !res.has("nodes"))
								  return fail_solicit("Unexpected result from listnodes.");

							  auto nodes = res["nodes"];
							  if (!nodes.is_array())
								  return fail_solicit("listnodes not array.");

							  auto proposals = std::deque<Ln::NodeId>();
							  for (auto node : nodes) {
								  if (!node.is_object() || !node.has("nodeid"))
									  continue;

								  // Does this node have a liquidity ad?
								  if (!node.has("option_will_fund"))
									  continue;

								  auto id = node["nodeid"];
								  if (!id.is_string())
									  continue;

								  auto nodeid = Ln::NodeId(std::string(id));

								  // Skip our own node
								  if (nodeid == self_id)
									  continue;

								  // Skip nodes we already have channels to
								  if (channels.find(nodeid) != channels.end())
									  continue;

								  proposals.push_back(nodeid);
							  }

							  /* Generate report.  */
							  auto os = std::ostringstream();
							  auto first = true;
							  for (auto const& p : proposals) {
								  if (first) {
									  os << "Proposing: ";
									  first = false;
								  } else
									  os << ", ";
								  os << p;
							  }
							  if (first)
								  os << "No proposals.";

							  auto act = Boss::log(bus, Info
												   , "ChannelFinderByLiquidityAds: %s"
												   , os.str().c_str()
								  );
							  auto f = [this](Ln::NodeId id) {
								  return bus.raise(Msg::ProposePatronlessChannelCandidate{
										  std::move(id)
									  });
							  };
							  std::vector<Ln::NodeId>
								  vector_proposals(proposals.begin(), proposals.end());
							  act += Ev::foreach(std::move(f), std::move(vector_proposals));

							  return act;
						  }).then([this]() {
							  running = false;
							  return Boss::log(bus, Info
											   , "ChannelFinderByListpays: "
											   "Run completion."
								  );
						  });
					  return Boss::concurrent(act);
				  });
}

Ev::Io<void> ChannelFinderByLiquidityAds::fail_solicit(std::string const& reason) {
	return Boss::log(bus, Error
					 , "ChannelFinderByLiquidityAds: Failed: %s"
					 , reason.c_str()
		).then([this]() {
			/* Failed!  */
			running = false;
			return Ev::lift();
		});
}

}}

