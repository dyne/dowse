#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <dnscrypt/plugin.h>
#include <ldns/ldns.h>

DCPLUGIN_MAIN(__FILE__);

const char *
dcplugin_description(DCPlugin * const dcplugin)
{
	return "Dowse plugin to filter dnscrypt queries";
}


const char *
dcplugin_long_description(DCPlugin * const dcplugin)
{
	return
		"This plugin checks all settings in Dowse and operates filtering on dnscrypt queries accordingly\n";
}

int
dcplugin_init(DCPlugin * const dcplugin, int argc, char *argv[])
{
	return 0;
}


int
dcplugin_destroy(DCPlugin * const dcplugin)
{
	void *data = dcplugin_get_user_data(dcplugin);
	return 0;
}


DCPluginSyncFilterResult dcplugin_sync_post_filter(DCPlugin *dcplugin, DCPluginDNSPacket *dcp_packet) {
	void *data = dcplugin_get_user_data(dcplugin);
	ldns_pkt *packet = NULL;
	DCPluginSyncFilterResult result = DCP_SYNC_FILTER_RESULT_OK;

	if (ldns_wire2pkt(&packet, dcplugin_get_wire_data(dcp_packet),
	                  dcplugin_get_wire_data_len(dcp_packet)) != LDNS_STATUS_OK) {
		return DCP_SYNC_FILTER_RESULT_ERROR;
	}

	ldns_pkt_free(packet);

	return DCP_SYNC_FILTER_RESULT_OK;


}
