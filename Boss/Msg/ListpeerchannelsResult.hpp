#ifndef BOSS_MSG_LISTPEERCHANNELSRESULT_HPP
#define BOSS_MSG_LISTPEERCHANNELSRESULT_HPP

#include"Jsmn/Object.hpp"

namespace Boss { namespace Msg {

/** struct Boss::Msg::ListpeerchannelsResult
 *
 * @brief announced during `init` to inform all
 * modules about `listpeerchannels` command result.
 * Also announced every 10 minutes.
 */
struct ListpeerchannelsResult {
	/* Known to be an array.  */
	Jsmn::Object peerchannels;
	/* Whether this listpeerchannels was performed during `init`
	 * or on the 10-minute timer.
	 */
	bool initial;
};

}}

#endif /* !defined(BOSS_MSG_LISTPEERCHANNELSRESULT_HPP) */
