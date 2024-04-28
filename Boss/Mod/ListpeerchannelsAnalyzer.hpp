#ifndef BOSS_MOD_LISTPEERCHANNELSANALYZER_HPP
#define BOSS_MOD_LISTPEERCHANNELSANALYZER_HPP

namespace S { class Bus; }

namespace Boss { namespace Mod {

/** class Boss::Mod::ListpeerchannelsAnalyzer
 *
 * @brief converts `Boss::Msg::ListpeerchannelsResult` to
 * `Boss::Msg::ListpeerchannelsAnalyzedResult`.
 */
class ListpeerchannelsAnalyzer {
public:
	ListpeerchannelsAnalyzer(S::Bus&);
};

}}

#endif /* !defined(BOSS_MOD_LISTPEERCHANNELSANALYZER_HPP) */
