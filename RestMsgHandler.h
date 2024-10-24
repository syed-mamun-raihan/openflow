#ifndef RESTMSGHANDLER_H
	#define RESTMSGHANDLER_H
/**
 * For commercial, educational or non-profit use please contact 
 * syed.raihan@gmail.com 
 * Otherwise use of this software is free to evaluate and test 
 * Copyright and Licenses owned by Syed Mamun Raihan 2018
 */

#include "Server.h"
#include "JsonController.h"

#include "OfDpDefine.hpp"
#include "OfConfig.hpp"
#include "OfDpLogicalSw.h"
#include "OfDpConfigApi.h"
#include "OfDataPlaneApi.h"
#include "json.h"

using namespace Mongoose;

/**
 * @class restMsgHandler RestMsgHandler.h
 * @brief accepts json rest requests, uses mongoose & jsoncpp
 */

class restMsgHandler : public JsonController
{
	public:
		/**
		 * @brief our handler for a GET request issued to the cofa/echo uri
		 * @param[in] request the clients request received by the server
		 * @param[in] response the servers response formatted in json by jsoncpp
		 */
		void getEchoHndlr(Request &request, JsonResponse &response);

		/**
		 * @param[in] request the clients request received by the server
		 * @param[in] response the servers response formatted in json by jsoncpp
		 * @brief our handler for a POST request issued to the cofa/logicalswitch uri
		*/
		void postLsHndlr(Request &request, JsonResponse &response);

		/**
		 * @param[in] request the clients request received by the server
		 * @param[in] response the servers response formatted in json by jsoncpp
		 * @brief our handler for a GET request issued to the cofa/logicalswitch uri
		 */
		void getLsHndlr(Request &request, JsonResponse &response);

		/**
		 * @param[in] request the clients request received by the server
		 * @param[in] response the servers response formatted in json by jsoncpp
		 * @brief our handler for a DELETE request issued to the cofa/logicalswitch uri
		 */
		void delLsHndlr(Request &request, JsonResponse &response);

		/**
		 * @param[in] request the clients request received by the server
		 * @param[in] response the servers response formatted in json by jsoncpp
		 * @brief our handler for a POST request issued to the cofa/controller uri
		 */
		void postCtrlHndlr(Request &request, JsonResponse &response);

		/**
		 * @param[in] request the clients request received by the server
		 * @param[in] response the servers response formatted in json by jsoncpp
		 * @brief our handler for a GET request issued to the cofa/controller uri
		 */
		void getCtrlHndlr(Request &request, JsonResponse &response);

		/**
		 * @param[in] request the clients request received by the server
		 * @param[in] response the servers response formatted in json by jsoncpp
		 * @brief our handler for a DELETE request issued to the cofa/controller uri
		 */
		void delCtrlHndlr(Request &request, JsonResponse &response);

		/**
		 * @brief simulates the dataplane, this is how cofa becomes aware of ports
		 * @param[in] request the request passed to the server by the client
		 * @param[in] response the json-formatted response to the client
		 */
		void addPortsHndlr(Request &request, JsonResponse &response);

		/**
		 * @brief simulates the dataplane, this is how cofa reports ports
		 * @param[in] request the request passed to the server by the client
		 * @param[in] response the json-formatted response to the client
		 */
		void getPortsHndlr(Request &request, JsonResponse &response);

		/**
		 * @brief simulates the dataplane, get a single port
		 * @param[in] request the request passed to the server by the client
		 * @param[in] response the json-formatted response to the client
		 */
		void getPortHndlr(Request &request, JsonResponse &response);

		/**
		 * @brief add or update a port, including port up/down
		 * @param[in] request the request passed to the server by the client
		 * @param[in] response the json-formatted response to the client
		 */
		void postPortHndlr(Request &request, JsonResponse &response);

		/**
		 * @brief delete a port
		 * @param[in] request the request passed to the server by the client
		 * @param[in] response the json-formatted response to the client
		 */
		void delPortHndlr(Request &request, JsonResponse &response);

		/**
		 * @brief Sets the counters of the flow, simulating traffic
		 * @param[in] request the request passed to the server by the client
		 * @param[in] response the json-formatted response to the client
		 */
		void setDataPlaneFlow(Request &request, JsonResponse &response);

		/**
		 * @brief retreives counters of the flows
		 * @param[in] request the request passed to the server by the client
		 * @param[in] response the json-formatted response to the client
		 */
		void getDataPlaneFlow(Request &request, JsonResponse &response);

		/** @brief creates the uris for the server,comes from mongoose
		 */
		void setup();

	protected:
		Json::Value parsed_json;
		Json::Value defVal;
		Json::Reader json_parser;
		/**
		 * @brief verifys the input values for logical switch creation
		 * @param[in] parsed_json the parsed json received by the server
		 */
		bool validLsData(Json::Value parsed_json);

		/**
		 * @brief verifys the input values for controller creation
		 * @param[in] parsed_json the parsed json received by the server
		 */
		bool validCtrlData(Json::Value parsed_json);

		/**
		 * @brief attaches or detaches a logical switch / controller
		 * @param[in] parsed_json jsons string received by the server
		 */
		bool modifyAttachment(Json::Value parsed_json);

		/**
		 * @brief attaches ports to a logical switch, cannot remove them from the system
		 * @param[in] parsed_json json string containing the list of ports and the name of the logical switch
		 */
		bool modifyPorts(Json::Value parsed_json, bool detach);
};
#endif
