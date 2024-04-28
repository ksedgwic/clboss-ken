#ifndef BOSS_MOD_LISTPEERCHANNELSANNOUNCER_HPP
#define BOSS_MOD_LISTPEERCHANNELSANNOUNCER_HPP

#include<memory>

namespace Boss { namespace ModG { class RpcProxy; }}
namespace S { class Bus; }

namespace Boss { namespace Mod {

/** class Boss::Mod::ListpeerchannelsAnnouncer
 *
 * @brief announces `listpeerchannels` at `init` and
 * every 10 minutes thereafter.
 */
class ListpeerchannelsAnnouncer {
private:
	S::Bus& bus;
	std::unique_ptr<Boss::ModG::RpcProxy> rpc;

	void start();

public:
	ListpeerchannelsAnnouncer() =delete;
	ListpeerchannelsAnnouncer(ListpeerchannelsAnnouncer const&) =delete;
	~ListpeerchannelsAnnouncer();
	ListpeerchannelsAnnouncer(S::Bus& bus);
};

}}

#endif /* BOSS_MOD_LISTPEERCHANNELSANNOUNCER_HPP */
