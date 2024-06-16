#include"Boss/Mod/ConnectFinderByHardcode.hpp"
#include"Boss/Msg/ProposeConnectCandidates.hpp"
#include"Boss/Msg/Init.hpp"
#include"Boss/Msg/Network.hpp"
#include"Boss/Msg/SolicitConnectCandidates.hpp"
#include"Boss/log.hpp"
#include"Boss/random_engine.hpp"
#include"S/Bus.hpp"
#include"Util/make_unique.hpp"
#include<map>
#include<string>
#include<vector>

namespace {

auto const all_nodes =
		std::map<Boss::Msg::Network, std::vector<std::string>>
{ { Boss::Msg::Network_Bitcoin
  , { "03e2408a49f07d2f4083a47344138ef89e7617e63919202c92aa8d49b574a560ae@65.108.246.14:19736"
    , "035e4ff418fc8b5554c5d9eea66396c227bd429a3251c8cbc711002ba215bfc226@170.75.163.209:9735"
    , "03abf6f44c355dec0d5aa155bdbdd6e0c8fefe318eff402de65c6eb2e1be55dc3e@3.132.230.42:9735"
    , "03b65c19de2da9d35895b37e6fa263cefbf8d184e6b61c0713747ebf12409d219f@164.90.241.111:9735"
    , "02d96eadea3d780104449aca5c93461ce67c1564e2e1d73225fa67dd3b997a6018@45.86.229.190:9736"
    , "031f2669adab71548fad4432277a0d90233e3bc07ac29cfb0b3e01bd3fb26cb9fa@44.242.118.94:9735"
    , "03cde60a6323f7122d5178255766e38114b4722ede08f7c9e0c5df9b912cc201d6@34.65.85.39:9745"
    , "02f1a8c87607f415c8f22c00593002775941dea48869ce23096af27b0cfdcc0b69@52.13.118.208:9735"
    , "0294ac3e099def03c12a37e30fe5364b1223fd60069869142ef96580c8439c2e0a@8.210.134.135:26658",
    }
  }
  , { Boss::Msg::Network_Testnet
    ,  { "03781444afe27babf724304c25370f195d9d955065d7894539ed7db5fe596badb8@138.197.57.50:19735"
       , "03e84a109cd70e57864274932fc87c5e6434c59ebb8e6e7d28532219ba38f7f6df@139.144.22.237:9735"
       , "03f060953bef5b777dc77e44afa3859d022fc1a77c55138deb232ad7255e869c00@212.46.38.66:9735"
       , "036497fee1a0b963a63f0b4374819d9016fcca9c09dbef4cb427a2d0146649b025@44.234.48.207:9735"
       , "028ec70462207b57e3d4d9332d9e0aee676c92d89b7c9fb0850fc2a24814d4d83c@71.171.123.161:9735"
       , "02eadbd9e7557375161df8b646776a547c5cbc2e95b3071ec81553f8ec2cea3b8c@18.118.231.3:9735"
       , "02312627fdf07fbdd7e5ddb136611bdde9b00d26821d14d94891395452f67af248@23.237.77.12:9735"
       , "02889be42fc32093d2dcbfa59369df262e3577b333d8a45e5859dcdd6a4139839a@[2a09:8280:1::42:a6f3]:9735"
       , "031c64a68e6d1b9e50711336d92b434c584ce668b2fae59ee688bd73713fee1569@186.154.207.228:9735"
       , "038863cf8ab91046230f561cd5b386cbff8309fa02e3f0c3ed161a3aeb64a643b9@203.132.94.196:9735"
    }
  }
  , { Boss::Msg::Network_Signet
    ,  { "02dadf6c28f3284d591cd2a4189d1530c1ff82c07059ebea150a33ab76e7364b4a@[2401:2500:102:3007:153:126:143:201]:9736"
       , "02dfb81e2f7a3c4c9e8a51b70ef82b4a24549cc2fab1f5b2fd636501774a918991@103.99.171.201:39735"
       , "03f1629af3101fcc56b7aac2667016be84e3defbf3d0c8719f836c9b41c9a57a43@103.99.170.206:39735"
       , "03ddab321b760433cbf561b615ef62ac7d318630c5f51d523aaf5395b90b751956@103.99.170.201:39735"
    }
  }
};

}

namespace Boss { namespace Mod {

class ConnectFinderByHardcode::Impl {
private:
	S::Bus& bus;

	std::vector<std::string> const *nodes;

	void start() {
		bus.subscribe<Msg::Init>([this](Msg::Init const& init) {
			auto it = all_nodes.find(init.network);
			if ( it == all_nodes.end()
			  || it->second.size() == 0
			   )
				return Boss::log( bus, Warn
						, "HardcodedSeeds: "
						  "Cannot seed by hardcoded: "
						  "No known nodes for "
						  "this network."
						);
			nodes = &it->second;
			return Ev::lift();
		});
		bus.subscribe<Msg::SolicitConnectCandidates>([this](Msg::SolicitConnectCandidates const& _) {
			return solicit();
		});
	}

	Ev::Io<void> solicit() {
		if (!nodes)
			return Ev::lift();
		/* Pick 2 nodes.  */
		auto dist = std::uniform_int_distribution<size_t>(
			0, nodes->size() - 1
		);

		auto selected = std::vector<std::string>();
		selected.push_back((*nodes)[dist(random_engine)]);
		selected.push_back((*nodes)[dist(random_engine)]);

		/* Raise it.  */
		return bus.raise(Msg::ProposeConnectCandidates{
			std::move(selected)
		});
	}

public:
	Impl(S::Bus& bus_) : bus(bus_), nodes(nullptr) { start(); }
};

ConnectFinderByHardcode::ConnectFinderByHardcode
		( S::Bus& bus
		) : pimpl(Util::make_unique<Impl>(bus))
		  { }
ConnectFinderByHardcode::ConnectFinderByHardcode
		( ConnectFinderByHardcode&& o
		) : pimpl(std::move(o.pimpl))
		  { }
ConnectFinderByHardcode::~ConnectFinderByHardcode() { }


}}
