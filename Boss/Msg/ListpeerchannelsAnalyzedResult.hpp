#ifndef BOSS_MSG_LISTPEERCHANNELSANALYZEDRESULT_HPP
#define BOSS_MSG_LISTPEERCHANNELSANALYZEDRESULT_HPP

#include"Ln/NodeId.hpp"
#include<set>

namespace Boss { namespace Msg {

/** struct Boss::Msg::ListpeerchannelsAnalyzedResult
 *
 * @brief message derived from `Boss::Msg::ListpeerchannelsResult`
 * with the peerchannels split up into bins: connected+channeled,
 * disconnected+channeled, connected+unchanneled,
 * disconnected+unchanneled.
 *
 * @desc "channeled" specifically means that there is at
 * least one channel that is in some `OPENINGD` or
 * `CHANNELD` state.
 */
struct ListpeerchannelsAnalyzedResult {
	std::set<Ln::NodeId> connected_channeled;
	std::set<Ln::NodeId> disconnected_channeled;
	std::set<Ln::NodeId> connected_unchanneled;
	std::set<Ln::NodeId> disconnected_unchanneled;
	/* Whether this `listpeerchannels` was performed during `init`
	 * or on the 10-minute timer.  */
	bool initial;
};g

}}

#endif /* !defined(BOSS_MSG_LISTPEERCHANNELSANALYZEDRESULT_HPP) */
