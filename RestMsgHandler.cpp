/**
 * For commercial, educational or non-profit use please contact 
 * syed.raihan@gmail.com 
 * Otherwise use of this software is free to evaluate and test 
 * Copyright and Licenses owned by Syed Mamun Raihan 2018
 */

#include "RestMsgHandler.h"
#include <string>
#include <boost/range/adaptor/map.hpp>
#include "openflow_1.3.h"

bool restMsgHandler::modifyAttachment(Json::Value parsed_json)
{
	OfDpControllerAttachmentT attachment;
	Json::Value names;
	if(parsed_json.isMember("attach")){
		names = parsed_json["attach"];
		attachment.command = OFDP_CONFIG_CREATE;
		attachment.role = OFDP_CHANNEL_ROLE_EQUAL;
        attachment.probeInterval = 15;
        attachment.timeoutInterval = 30;
        attachment.maxRetries = (uint32_t)(-1);
	}
	if(parsed_json.isMember("detach")){
		names = parsed_json["detach"];
		attachment.command = OFDP_CONFIG_REMOVE;
	}
    if(names.isMember("controller") && names.isMember("logicalSwitch")){
        attachment.name = names["controller"].asString();
        attachment.lsName = names["logicalSwitch"].asString();
		if(OfDpConfigApi::getInstance()->Update(attachment) == OFDP_OK){
            return true;
        } else {
            return false;
        }
	}
	return false;
}

bool restMsgHandler::modifyPorts(Json::Value data, bool detach)
{
	std::lock_guard<std::recursive_mutex> lock(OfDpConfigApi::getInstance()->GetPorts().mutex());
	if(!(data["ports"].isArray()))
		return false;
	OfDpPortAttachmentT attachment;
	attachment.command = detach ? OFDP_CONFIG_REMOVE : OFDP_CONFIG_CREATE;
	std::string verb = detach ? " detached" : " attached";
    OfDpReturn retVal;
	for(unsigned int i = 0; i < data["ports"].size(); i++){
		attachment.name = data["ports"][i].asString();
		attachment.lsName = data["name"].asString();
		attachment.port_no = data["ports"][i].asInt();
		retVal = OfDpConfigApi::getInstance()->Update(attachment);
		if(retVal == OFDP_OK){
			continue;
		} else {
			std::cout <<"return code: "<<to_string(retVal)<<std::endl;
			std::cout << "Could not Attach port " <<
				attachment.port_no << std::endl;
			std::cout << "to LS: "<< data["name"].asString() << std::endl;
			std::cout << " error code : " << to_string(retVal);
			return false;
		}
	}
    return true;
} // modifyPorts



bool restMsgHandler::validLsData(Json::Value parsed_json)
{
    // required fields to create a logical switch: name, dpid,version,
    if(parsed_json.isMember("name") &&
            parsed_json.isMember("version") &&
            parsed_json.isMember("DPID")) {
        return true;
    } else {
        return false;
    }
}

bool restMsgHandler::validCtrlData(Json::Value parsed_json)
{
    if(parsed_json.isMember("name") &&
            parsed_json.isMember("port") &&
            parsed_json.isMember("ipAddress")){
        return true;
    } else{
        return false;
    }
}

void restMsgHandler::getEchoHndlr(Request &request, JsonResponse &response)
{
    json_parser.parse(request.getData(), parsed_json, false);
    //status coming soon
    std::cout << "sending echo!!!" << std::endl;
    std::cout << parsed_json["echo"] << std::endl;

    response["echo"]  = "echo";
}

void restMsgHandler::postLsHndlr(Request &request, JsonResponse &response)
{
    json_parser.parse(request.getData(), parsed_json, false);

    if(validLsData(parsed_json)){
        std::cout << "VALID LS DATA" << std::endl;
        OfDpLogicalSwT ls;
        ls.name = parsed_json["name"].asString();
        std::cout << "ls.name is:" << ls.name << std::endl;
        ls.version = static_cast<OfDpVersion>(parsed_json["version"].asInt());

        // accepts DPID in xx:xx:xx:xx:xx:xx:xx:xx
        // sets it into a single 64 bit number
        char *save_ptr=0;
        uint8_t i = 0;
        char* dpid_str = const_cast<char*>(parsed_json["DPID"].asCString());
        char* token = strtok_r(dpid_str, ":", &save_ptr);
        while(token != NULL && i < OFDP_DPID_LEN){
            ls.dpId[i++] = std::stoi(token,0,16);
            token = strtok_r(NULL,":",&save_ptr);
        }
        *(uint64_t*)(ls.dpId.data()) = ofp_message::ntohll(*(uint64_t*)(ls.dpId.data()));
        std::cout << " dpid numId " << *(uint64_t*)(ls.dpId.data()) << "\n";
        ls.adminState = parsed_json["adminState"].asInt();
        ls.negotiatedVersion = ls.version;
        ls.command = OFDP_CONFIG_CREATE;

        if(OfDpConfigApi::getInstance()->Update(ls) == OFDP_OK){
            const OfDpLogicalSwT *datapath = OfDpConfigApi::getInstance()->GetLogicalSw(ls.name);

            // later on this will be the json representation of the ls we created
            if(datapath) {
                response["status"] = "success";
                response["name"] = datapath->name;
                response["version"] = to_string(datapath->version);
            }//datapath
            std::cout << "Logical Switch creation WORKED" << std::endl;
        } else {
			response["status"] = "failed";
		}
    }
    if(parsed_json.isMember("attach") || parsed_json.isMember("detach")){
		// get the right verbage for the detail strings
		std::string detail_suc_str = parsed_json.isMember("attach") ?
				"LS attached to CTRL" : "LS detached from CTRL";
		std::string detail_fail_str = parsed_json.isMember("attach") ?
				"LS not attached to CTRL" : "LS not detached from CTRL";
        if(modifyAttachment(parsed_json)){
            response["status"] = "success";
            response["detail"] = detail_suc_str;
        } else{
            response["status"] = "failed";
            response["detail"] = detail_fail_str;
        }
    }
    if(parsed_json.isMember("ports")){
        if(modifyPorts(parsed_json, false)){
            response["status"] = "success";
            response["detail"] = "ports attached";
        } else{
            response["status"] = "failed";
            response["detail"] = "ports not attached";
        }
    }
	if(!(validLsData(parsed_json))&&
            !(parsed_json.isMember("attach")) &&
            !(parsed_json.isMember("detach")) &&
            !(parsed_json.isMember("ports"))){
        std::cout << "FAIL WHALE!!!" << std::endl;
        response["status"] = "failed";
        response["detail"] = "all required fields not supplied";
    }


}// postLsHndlr

void restMsgHandler::getLsHndlr(Request &request, JsonResponse &response)
{
    // check that the request has the name
    json_parser.parse(request.getData(), parsed_json, false);
    if(parsed_json.isMember("name")){
        std::cout << "Querying the Database" << std::endl;
        // call into OfDpConfigApi to get the ls with the name
        const OfDpLogicalSwT *datapath = OfDpConfigApi::getInstance()->GetLogicalSw(parsed_json["name"].asString());
        if(datapath) {
            std::cout << "Database Hit on " << to_string(datapath->version)<<std::endl;
            response["name"] = datapath->name;
            response["version"] = to_string(datapath->version);
			std::list<OfDpPortAttachmentT> ports;
			std::list<OfDpControllerAttachmentT> ctrls;
			OfDpConfigApi::getInstance()->Get(parsed_json["name"].asString(),
				   ports);
			response["attached ports"] = (int)(ports.size());
			for( const auto& port: ports ){
				std::cout << port.name << std::endl;
				response[port.name] = port.port_no;
			}
			OfDpConfigApi::getInstance()->Get(parsed_json["name"].asString(),
				   ctrls);
			response["attached ctrls"] = (int)(ctrls.size());
			for(const auto& ctrl: ctrls){
				std::cout << ctrl.name << std::endl;
				response["ctrl"] = ctrl.name;
			}
			response["attached flows"] = (int)OfDpConfigApi::getInstance()->GetFlowCount(parsed_json["name"].asString());
        } else {
            std::cout << "Database Miss" << std::endl;
            response["status"] = "not found";
        }
    } else
        response["status"] = "error - supply name for query";
}

void restMsgHandler::delLsHndlr(Request &request, JsonResponse &response)
{
    json_parser.parse(request.getData(), parsed_json, false);
	if(parsed_json.isMember("name") && parsed_json.isMember("ports")){
		if(modifyPorts(parsed_json, true)){
			response["ports"] = "detached";
			response["status"] = "success";
		} else{
			response["status"] = "failed";
		}
	}
    if(parsed_json.isMember("name") && (!parsed_json.isMember("ports"))){
        OfDpLogicalSwT ls;
        ls.name = parsed_json["name"].asString();
        ls.command = OFDP_CONFIG_REMOVE;
        OfDpReturn retVal = OfDpConfigApi::getInstance()->Update(ls);
        if( retVal == OFDP_OK) {
            response["status"] = "success";
            response["name"] = parsed_json["name"];
            response["of_code"] = to_string(retVal);
            response["detail"] = "Logical Switch deleted";
        } else {
            response["status"] = "failed";
            response["name"] = parsed_json["name"];
            response["of_code"] = to_string(retVal);
            response["detail"] = "Logical Switch not deleted";
        }
    }
   if(!(parsed_json.isMember("name")) && !(parsed_json.isMember("ports"))){
        response["status"] = "failed";
        response["detail"] = "needed information not supplied";
   }
}

void restMsgHandler::postCtrlHndlr(Request &request, JsonResponse &response)
{
    json_parser.parse(request.getData(), parsed_json, false);
    if(validCtrlData(parsed_json)){
        std::cout << "VALID CTRL DATA" << std::endl;
        OfDpControllerT controller;
        controller.name = parsed_json["name"].asString();
        controller.command = OFDP_CONFIG_CREATE;
        controller.ipAddress = parsed_json["ipAddress"].asString();
        controller.port = parsed_json["port"].asString();
        if(OfDpConfigApi::getInstance()->Update(controller) == OFDP_OK) {
            response["status"] = "success";
            response["name"] = controller.name;
        }
    }
    if(parsed_json.isMember("attach") || parsed_json.isMember("detach")){

		// get the right verbage for the detail strings
		std::string detail_suc_str = parsed_json.isMember("attach") ?
				"LS attached to CTRL" : "LS detached from CTRL";
		std::string detail_fail_str = parsed_json.isMember("attach") ?
				"LS not attached to CTRL" : "LS not detached from CTRL";

        if(modifyAttachment(parsed_json)){
            response["status"] = "success";
            response["detail"] = detail_suc_str;
        } else{
            response["status"] = "failed";
            response["detail"] = detail_fail_str;
        }
    }
	if(!(validCtrlData(parsed_json)) &&
				!(parsed_json.isMember("attach")) &&
				!(parsed_json.isMember("detach"))){
        response["status"] = "failed";
        response["detail"] = "required info not supplied for ctrl creation";

	}
}//postCtrlHndlr

void restMsgHandler::getCtrlHndlr(Request &request, JsonResponse &response)
{
	json_parser.parse(request.getData(), parsed_json, false);
	if(parsed_json.isMember("name")){

		const OfDpControllerT *datapath = OfDpConfigApi::getInstance()->GetController(
				parsed_json["name"].asString());
		if(datapath) {
			std::cout << "Database Hit on " << datapath->name <<std::endl;
			response["name"] = datapath->name;
			response["port"] = datapath->port;
			response["IpAddress"] = datapath->ipAddress;
			std::list<OfDpLogicalSwT> switches;
			OfDpConfigApi::getInstance()->Get(parsed_json["name"].asString(),
					switches);
			response["attached switches"] = (int)(switches.size());
			for(const auto& ls: switches){
				std::cout << ls.name << std::endl;
				response["switch"] = ls.name;
			}
		} else {
			std::cout << "Database Miss" << std::endl;
			response["status"] = "not found";
		}
	} else
		response["status"] = "error - supply name for controller query";



}

void restMsgHandler::delCtrlHndlr(Request &request, JsonResponse &response)
{
    json_parser.parse(request.getData(), parsed_json, false);
    if(parsed_json.isMember("name")){
        OfDpControllerT controller;
        controller.name = parsed_json["name"].asString();
        controller.command = OFDP_CONFIG_REMOVE;
        OfDpReturn retVal = OfDpConfigApi::getInstance()->Update(controller);
        if( retVal == OFDP_OK) {
            response["status"] = "success";
            response["name"] = parsed_json["name"];
            response["of_code"] = to_string(retVal);
            response["detail"] = "Controller deleted";
        } else {
            response["status"] = "failed";
            response["name"] = parsed_json["name"];
            response["of_code"] = to_string(retVal);
            response["detail"] = "Controller not deleted";
        }
    } else
        response["status"] = "error - supply name to delete";
}

void restMsgHandler::addPortsHndlr(Request &request, JsonResponse &response)
{
	// Default port values for creating new ports
    OfDataPlaneApi* dpstandalone = OfDpConfigApi::getInstance()->getDataPlaneApiPtr();

	json_parser.parse(request.getData(), parsed_json, false);
	if(parsed_json.isMember("ports") && parsed_json["ports"].isArray()){
		OfDpReturn retVal=OFDP_OK;
		int error_sum = 0;
		for(unsigned int i = 0; i < parsed_json["ports"].size(); i++){
			uint32_t port_no = parsed_json["ports"][i].asInt();
			char name[32];
			OfDpPortInfoT portInfo;portInfo.port_no = port_no;

			snprintf(name, sizeof(name), "%u", port_no);  // default port name = port_no
			bool found = dpstandalone->GetPort(portInfo);
			if (!found)
			{
				portInfo.name = name;
				portInfo.port_no = port_no;
				portInfo.command = OFDP_CONFIG_CREATE;
				OfDpConfigApi::getInstance()->Update(portInfo);
				retVal = OFDP_OK;
			}
			else
			{
				retVal = OFDP_ENTRY_EXISTS;
				error_sum++;
			}
			response[name] = to_string(retVal);
		}
		if(error_sum){
			response["status"] = "failed";
			response["details"] = "Non OFDP_OK values returned, check details";
		} else
			response["status"] = "success";
            response["of_code"] = to_string(retVal);
	}
}

void restMsgHandler::getPortsHndlr(Request &request, JsonResponse &response)
{
	std::lock_guard<std::recursive_mutex> lock(OfDpConfigApi::getInstance()->GetPorts().mutex());
	response["number of ports"] = (int)OfDpConfigApi::getInstance()->GetPorts().size();
	for(const auto& port : OfDpConfigApi::getInstance()->GetPorts()| boost::adaptors::map_values){
		response[port->name] = port->port_no;
	}
}

void restMsgHandler::getPortHndlr(Request &request, JsonResponse &response)
{
    OfDataPlaneApi* dpstandalone = OfDpConfigApi::getInstance()->getDataPlaneApiPtr();

    json_parser.parse(request.getData(), parsed_json, false);
	std::lock_guard<std::recursive_mutex> lock(OfDpConfigApi::getInstance()->GetPorts().mutex());

    if (!parsed_json.isMember("port"))
    {
		response["status"] = "error - missing 'port' number";
		return;
    }

	const uint32_t port_no = parsed_json["port"].asInt();
	OfDpPortInfoT portInfo;portInfo.port_no = port_no;
	const bool found = dpstandalone->GetPort(portInfo);
	if (!found)
	{
		response["status"] = "error - port not found";
		return;
	}

	char hw_addr[32];
	snprintf(hw_addr, sizeof(hw_addr), "%02x:%02x:%02x:%02x:%02x:%02x",
			 portInfo.hw_addr[0],
			 portInfo.hw_addr[1],
			 portInfo.hw_addr[2],
			 portInfo.hw_addr[3],
			 portInfo.hw_addr[4],
			 portInfo.hw_addr[5]);

	response["port"]       = portInfo.port_no;
	response["name"]       = portInfo.name;
	response["hw_addr"]    = hw_addr;
	response["config"]     = portInfo.config;
	response["state"]      = portInfo.state;
	response["curr"]       = portInfo.curr;
	response["advertised"] = portInfo.advertised;
	response["supported"]  = portInfo.supported;
	response["peer"]       = portInfo.peer;
	response["curr_speed"] = portInfo.curr_speed;
	response["max_speed"]  = portInfo.max_speed;
	response["status"]     = "success";
}

void restMsgHandler::postPortHndlr(Request &request, JsonResponse &response)
{
    json_parser.parse(request.getData(), parsed_json, false);
	std::lock_guard<std::recursive_mutex> lock(OfDpConfigApi::getInstance()->GetPorts().mutex());

    OfDataPlaneApi* dpstandalone = OfDpConfigApi::getInstance()->getDataPlaneApiPtr();
    if (parsed_json.isMember("port"))
    {
		OfDpPortInfoT newPort;newPort.port_no = parsed_json["port"].asInt();
		bool found = dpstandalone->GetPort(newPort);
		if (!found)
		{
			char name[32];
			snprintf(name, sizeof(name), "%u", newPort.port_no);  // default port name = port_no
			newPort.command = OFDP_CONFIG_CREATE;
			newPort.name    = name;
		}else{
            newPort.command = OFDP_CONFIG_MODIFY;
        }

		if (parsed_json.isMember("name"))
		{
			newPort.name = parsed_json["name"].asString().c_str();
		}

		if (parsed_json.isMember("hw_addr"))
		{
			sscanf(parsed_json["hw_addr"].asString().c_str(),
				   "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
				   &newPort.hw_addr[0],
				   &newPort.hw_addr[1],
				   &newPort.hw_addr[2],
				   &newPort.hw_addr[3],
				   &newPort.hw_addr[4],
				   &newPort.hw_addr[5]);
		}

		if (parsed_json.isMember("down"))
		{
			newPort.state = (newPort.state & ~OpenflowV4::OFPPS_LINK_DOWN)
					      | (parsed_json["down"].asInt() ? OpenflowV4::OFPPS_LINK_DOWN : 0);
		}

		if (parsed_json.isMember("blocked"))
		{
			newPort.state = (newPort.state & ~OpenflowV4::OFPPS_BLOCKED)
					      | (parsed_json["blocked"].asInt() ? OpenflowV4::OFPPS_BLOCKED : 0);
		}

		if (parsed_json.isMember("live"))
		{
			newPort.state = (newPort.state & ~OpenflowV4::OFPPS_LIVE)
					      | (parsed_json["live"].asInt() ? OpenflowV4::OFPPS_LIVE : 0);
		}

		if (parsed_json.isMember("curr"))
		{
			newPort.curr = parsed_json["curr"].asInt();
		}

		if (parsed_json.isMember("advertised"))
		{
			newPort.advertised = parsed_json["advertised"].asInt();
		}

		if (parsed_json.isMember("supported"))
		{
			newPort.supported = parsed_json["supported"].asInt();
		}

		if (parsed_json.isMember("peer"))
		{
			newPort.peer = parsed_json["peer"].asInt();
		}

		if (parsed_json.isMember("curr_speed"))
		{
			newPort.curr_speed = parsed_json["curr_speed"].asInt();
		}

		if (parsed_json.isMember("max_speed"))
		{
			newPort.max_speed = parsed_json["max_speed"].asInt();
		}

		char hw_addr[32];
		snprintf(hw_addr, sizeof(hw_addr), "%02x:%02x:%02x:%02x:%02x:%02x",
				 newPort.hw_addr[0], newPort.hw_addr[1], newPort.hw_addr[2],
				 newPort.hw_addr[3], newPort.hw_addr[4], newPort.hw_addr[5]);

		/***
		std::cout << "calling OfDataPlaneStandAlone::SetPort"
				  << ", command="	          << newPort.command
				  << ", portNo="       << dec << newPort.port_no
				  << ", name="		 		  << newPort.name
				  << ", hw_addr="             << hw_addr
				  << ", config=0x"     << hex << newPort.config
				  << ", state=0x"      << hex << newPort.state
				  << ", curr=0x"       << hex << newPort.curr
				  << ", advertised=0x" << hex << newPort.advertised
				  << ", supported=0x"  << hex << newPort.supported
				  << ", peer=0x"       << hex << newPort.peer
				  << ", curr_speed="   << dec << newPort.curr_speed
				  << ", max_speed="    << dec << newPort.max_speed
				  << endl;
		***/

		dpstandalone->PortEventCallback(newPort);
		response["status"] = "success";
	}
    else
    {
		response["status"] = "error - missing 'port' number";
    }
}

void restMsgHandler::delPortHndlr(Request &request, JsonResponse &response)
{
    json_parser.parse(request.getData(), parsed_json, false);

    if (parsed_json.isMember("port"))
    {
        OfDataPlaneApi* dpstandalone = OfDpConfigApi::getInstance()->getDataPlaneApiPtr();
        OfDpPortInfoT port;
    	port.port_no = parsed_json["port"].asInt();
        dpstandalone->GetPort(port);
        port.command = OFDP_CONFIG_REMOVE;

    	bool rc = OfDpConfigApi::getInstance()->
							getDataPlaneApiPtr()->PortEventCallback(port);
		if (rc)
			response["status"] = "success";
		else
			response["status"] = "error - port not found";
    }
}

void restMsgHandler::getDataPlaneFlow(Request &request, JsonResponse &response)
{
	json_parser.parse(request.getData(), parsed_json, false);

	OfDpFlowT incoming, outgoing;
	OfDpBucketCounterT counters;
	OfDpResponseT of_response;
	OfDpReturn ret = OFDP_ENTRY_NOT_EXIST;
	if(parsed_json.isMember("flow") && parsed_json["flow"].isObject()){

		Json::Value flow = parsed_json["flow"];

		incoming.table_id = flow["table_id"].asUInt();
		incoming.priority = flow["priority"].asUInt();
		incoming.match.in_port = flow["in_port"].asUInt();
		incoming.match.dl_vlan = flow["vlan"].asUInt();
		incoming.match.dl_type = std::stoi(flow["ethertype"].asString(),0,16);


		char mac_str[OFDP_MAC_LEN * 2 + 5 ]; //*2 for str +5 for :
		strcpy(mac_str,flow["src_mac"].asCString());
		char* token, *save_ptr;
		token = strtok_r(mac_str, ":", &save_ptr);
		int i = 0;
		while(token != NULL){
			incoming.match.dl_src[i] = std::stoi(token,0,16);
			i++;
			token = strtok_r(NULL,":", &save_ptr);
		}
		strcpy(mac_str,flow["dst_mac"].asCString());
		token = strtok_r(mac_str, ":",&save_ptr);
		i = 0;
		while(token != NULL){
			incoming.match.dl_dst[i] = std::stoi(token,0,16);
			i++;
			token = strtok_r(NULL,":", &save_ptr);
		}

/*		ret = static_cast<OfDataPlaneStandAlone*>(OfDpConfigApi::getInstance()->
				getDataPlaneApiPtr())->GetRecordFromHash(incoming,
				outgoing, counters, of_response);
*/
		if(ret == OFDP_ENTRY_NOT_EXIST){
			response["status"] = "failed";
			response["detail"] = "entry did not exist";
			return;
		}
		std::stringstream mac_addr;
		response["flow"]["table_id"] = outgoing.table_id;
		response["flow"]["priority"] = outgoing.priority;
		response["flow"]["in_port"] = outgoing.match.in_port;
		response["flow"]["vlan"] = outgoing.match.dl_vlan;
		response["flow"]["ethertype"] = outgoing.match.dl_type;
		for(int i = 0; i < OFDP_MAC_LEN; i++){
			 mac_addr << std::hex<< static_cast<int>(outgoing.match.dl_src[i]) << ":";
		}
		response["flow"]["src_mac"] = mac_addr.str();

		mac_addr.str("");
		for(int i = 0; i < OFDP_MAC_LEN; i++){
			mac_addr << std::hex<< static_cast<int>(outgoing.match.dl_dst[i]) << ":";
		}
		response["flow"]["dst_mac"] = mac_addr.str();
		response["flow"]["byte_count"] = (Json::Value::UInt64)(counters.byte_count);
		response["flow"]["packet_count"] = (Json::Value::UInt64)(counters.packet_count);
		return;

	}

}
void restMsgHandler::setDataPlaneFlow(Request &request, JsonResponse &response)
{

	OfDpReturn ret(OFDP_ENTRY_NOT_EXIST);
	OfDpFlowT incoming;
	OfDpResponseT of_response;
	OfDpBucketCounterT counters;

	json_parser.parse(request.getData(), parsed_json, false);
	if(parsed_json.isMember("flow") &&
	   parsed_json["flow"].isObject())
	{

		Json::Value flow = parsed_json["flow"];
		const OfDpLogicalSwT *ls = OfDpConfigApi::getInstance()->GetLogicalSw(flow["lsName"].asString());
		incoming.table_id = flow["table_id"].asUInt();
		incoming.priority = flow["priority"].asUInt();
		incoming.match.in_port = flow["in_port"].asUInt();
		incoming.match.dl_vlan = flow["vlan"].asUInt();
		incoming.match.dl_type = std::stoi(flow["ethertype"].asString(),0,16);
		incoming.command = static_cast<OfDpFlowCommand>(flow["command"].asUInt());
		counters.byte_count = (uint64_t)flow["byte_count"].asUInt64();
		counters.packet_count = (uint64_t)flow["packet_count"].asUInt64();
		std::cout << " byte_count " << counters.byte_count << " packet_count " << counters.packet_count << std::endl;
		char mac_str[OFDP_MAC_LEN * 2 + 5 ]; //*2 for str +5 for :
		strcpy(mac_str,flow["src_mac"].asCString());
		char* token, *save_ptr;
		token = strtok_r(mac_str, ":",&save_ptr);
		int i = 0;
		while(token != NULL){
			incoming.match.dl_src[i] = std::stoi(token,0,16);
			i++;
			token = strtok_r(NULL,":", &save_ptr);
		}
		strcpy(mac_str,flow["dst_mac"].asCString());
		token = strtok_r(mac_str, ":", &save_ptr);
		i = 0;
		while(token != NULL){
			incoming.match.dl_dst[i] = std::stoi(token,0,16);
			i++;
			token = strtok_r(NULL,":", &save_ptr);
		}
		incoming.num_actions = 1;
		incoming.out_port = flow["out_port"].asUInt();
		if (!ls) std::cout << " lsName is incorrect \n";

/*		ret = static_cast<OfDataPlaneStandAlone*>(OfDpConfigApi::getInstance()->
				getDataPlaneApiPtr())->SetRecordInHash(incoming,
													counters, of_response); */
		if(ret == OFDP_ENTRY_NOT_EXIST){
			response["status"] = "failed";
			response["detail"] = "entry did not exist";
		} else{
			response["status"] = "success";
            response["of_code"] = to_string(ret);
		}
	}
}

void restMsgHandler::setup()
{
    // Example of prefix, putting all the urls into "/cofa"
    setPrefix("/cofa");

    addRouteResponse("GET", "/echo", restMsgHandler,
            getEchoHndlr, JsonResponse);

    // logical switch route additions
    addRouteResponse("POST", "/logicalSwitch", restMsgHandler,
            postLsHndlr, JsonResponse);
    addRouteResponse("GET", "/logicalSwitch", restMsgHandler,
            getLsHndlr, JsonResponse);
    addRouteResponse("DELETE", "/logicalSwitch", restMsgHandler,
            delLsHndlr, JsonResponse);
    // controller route additions
    addRouteResponse("POST", "/controller", restMsgHandler,
            postCtrlHndlr, JsonResponse);
    addRouteResponse("GET", "/controller", restMsgHandler,
            getCtrlHndlr, JsonResponse);
    addRouteResponse("DELETE", "/controller", restMsgHandler,
            delCtrlHndlr, JsonResponse);
	// dataplane
	addRouteResponse("POST","/dataplane/ports",restMsgHandler,addPortsHndlr,
			JsonResponse);
	addRouteResponse("GET","/dataplane/ports",restMsgHandler,getPortsHndlr,
			JsonResponse);
	addRouteResponse("POST","/dataplane/port",restMsgHandler,postPortHndlr,
			JsonResponse);
	addRouteResponse("GET","/dataplane/port",restMsgHandler,getPortHndlr,
			JsonResponse);
	addRouteResponse("DELETE","/dataplane/port",restMsgHandler,delPortHndlr,
			JsonResponse);
	addRouteResponse("GET","/dataplane/flows",restMsgHandler,getDataPlaneFlow,
			JsonResponse);
	addRouteResponse("POST","/dataplane/flows",restMsgHandler,setDataPlaneFlow,
			JsonResponse);
}
