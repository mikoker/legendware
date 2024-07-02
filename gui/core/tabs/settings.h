#pragma once
#include <string>

namespace core::tabs::settings
{
	/* configs tab */
	void configs_first_tab();
	void configs_second_tab();

	void configs_invites_tab();
	void configs_share_tab();

	bool configs_invite_popup();
	void configs_invite_accept_popup();


	void configs_link_accept_popup();
	/* scripts tab */
	void scripts_first_tab(std::string& script_name);
	void scripts_second_tab();

	extern bool share_is_open;
}
