#include "approver.h"

int main()
{
	CStatistics		appStat;  // --- CStatistics must be firts statement to measure end2end param's
	CCgi			indexPage(EXTERNAL_TEMPLATE);
	CUser			user;
	string			action, partnerID;
	CMysql			db;
	struct timeval	tv;

	{
		MESSAGE_DEBUG("", action, __FILE__);
	}

	signal(SIGSEGV, crash_handler);

	gettimeofday(&tv, NULL);
	srand(tv.tv_sec * tv.tv_usec * 100000);

	try
	{

	indexPage.ParseURL();
	indexPage.AddCookie("lng", "ru", "", "", "/");

	if(!indexPage.SetTemplate("index.htmlt"))
	{
		MESSAGE_DEBUG("", action, "template file was missing");
		throw CException("Template file was missing");
	}

	if(db.Connect(DB_NAME, DB_LOGIN, DB_PASSWORD) < 0)
	{
		MESSAGE_DEBUG("", action, "Can not connect to mysql database");
		throw CExceptionHTML("MySql connection");
	}
	db.Query("set names " + DB_CHARSET);

	indexPage.SetDB(&db);


	action = CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("action"));
	{
		MESSAGE_DEBUG("", "", " action = " + action);
	}

	// --- generate common parts
	{
		// --- it has to be run before session, because session relay upon Apache environment variable
		if(RegisterInitialVariables(&indexPage, &db, &user))
		{
		}
		else
		{
			MESSAGE_ERROR("", "", "RegisterInitialVariables failed, throwing exception");
			throw CExceptionHTML("environment variable error");
		}

		//------- Generate session
		action = GenerateSession(action, &indexPage, &db, &user);
	}
// ------------ end generate common parts

	MESSAGE_DEBUG("", "", "pre-condition if(action == \"" + action + "\")");

	if((action.length() > 10) && (action.compare(action.length() - 9, 9, "_template") == 0))
	{
		ostringstream	ost;
		string			strPageToGet, strFriendsOnSinglePage;

		{
			MESSAGE_DEBUG("", action, "start");
		}

		if(user.GetLogin() == "Guest")
		{
			{
				MESSAGE_DEBUG("", action, "re-login required");
			}

			indexPage.Redirect("/" + GUEST_USER_DEFAULT_ACTION + "?rand=" + GetRandom(10));
		}
		else
		{
			string		template_name = action.substr(0, action.length() - 9) + ".htmlt";

			if(!indexPage.SetTemplate(template_name))
			{
				MESSAGE_DEBUG("", action, "can't find template " + template_name);
			} // if(!indexPage.SetTemplate("my_network.htmlt"))
		}

		{
			MESSAGE_DEBUG("", action, "finish");
		}
	}

	if(action == "AJAX_getDashboardData")
	{
		string			strPageToGet, strFriendsOnSinglePage;
		ostringstream	ostResult;

		{
			MESSAGE_DEBUG("", action, "start");
		}

		ostResult.str("");
		if(user.GetLogin() == "Guest")
		{
			{
				MESSAGE_DEBUG("", action, "re-login required");
			}

			ostResult << "{\"result\":\"error\",\"description\":\"re-login required\"}";
		}
		else
		{
			if(user.GetType() == "approver")
			{
				string		template_name = "json_response.htmlt";
				bool		successFlag = true;
				string		pending_timecards = "";

				if(successFlag)
				{
					ostResult << "{"
									"\"status\":\"success\","
									"\"number_of_pending_timecards\":" << GetNumberOfTimecardsInPendingState(&db, &user) << ","
									"\"number_of_pending_bt\":" << GetNumberOfBTInPendingState(&db, &user) << ""
								"}";
				}
				else
				{
					MESSAGE_ERROR("", action, "dashboard data didn't gathered completely");
					ostResult << "{\"status\":\"error\",\"description\":\"Ошибка построения панели управления\"}";
				}

				indexPage.RegisterVariableForce("result", ostResult.str());

				if(!indexPage.SetTemplate(template_name))
				{
					MESSAGE_DEBUG("", action, "can't find template " + template_name);
				}
			}
			else
			{
				MESSAGE_ERROR("", action, "user.id(" + user.GetID() + ") not an approver(" + user.GetType() + ")");
				ostResult << "{\"status\":\"error\",\"description\":\"Вы не наначены на согласование\"}";
			}
		}


		{
			MESSAGE_DEBUG("", action, "finish");
		}
	}

// TODO: remove Dec 1
/*
	if(action == "AJAX_getApprovalsList")
	{
		string			strPageToGet, strFriendsOnSinglePage;
		ostringstream	ostResult;

		{
			MESSAGE_DEBUG("", action, "start");
		}

		ostResult.str("");
		if(user.GetLogin() == "Guest")
		{
			{
				MESSAGE_DEBUG("", action, "re-login required");
			}

			ostResult << "{\"result\":\"error\",\"description\":\"re-login required\"}";
		}
		else
		{
			string			template_name = "json_response.htmlt";
			string			object = CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("object"));;
			string			filter = CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("filter"));;

			if((object == "timecard") || (object == "bt"))
			{
				if((user.GetType() == "agency") || (user.GetType() == "approver"))
				{
					string	temp = GetObjectsSOW_Reusable_InJSONFormat(object, filter, &db, &user);

					if(temp.length())
					{
						ostResult	<< "{"
									<< "\"status\":\"success\","
									<< temp
									<< "}";
					}
					else
					{
						MESSAGE_ERROR("", action, "error get reusable data");
						ostResult << "{\"status\":\"error\",\"description\":\"Ошибка получения данных\"}";
					}

				}
				else
				{
					MESSAGE_ERROR("", action, "user(" + user.GetID() + ") neither approver, nor agency");
					ostResult << "{\"status\":\"error\",\"description\":\"Вы не должны согласовывать\"}";
				}
			}
			else
			{
				ostResult << "{\"status\":\"error\",\"description\":\"Неизвестный тип обьекта\"}";
				MESSAGE_ERROR("", action, "unknown object type (" + object + ")");
			}

			indexPage.RegisterVariableForce("result", ostResult.str());

			if(!indexPage.SetTemplate(template_name))
			{
				MESSAGE_DEBUG("", action, "can't find template " + template_name);
			} // if(!indexPage.SetTemplate("my_network.htmlt"))
		}


		{
			MESSAGE_DEBUG("", action, "finish");
		}
	}
*/
	if(action == "AJAX_getTimecardEntry")
	{
		string			strPageToGet, strFriendsOnSinglePage;
		ostringstream	ostResult;

		{
			MESSAGE_DEBUG("", action, "start");
		}

		ostResult.str("");
		if(user.GetLogin() == "Guest")
		{
			{
				MESSAGE_DEBUG("", action, "re-login required");
			}

			ostResult << "{\"result\":\"error\",\"description\":\"re-login required\"}";
		}
		else
		{
			string			template_name = "json_response.htmlt";
			string			timecard_id = "";

			timecard_id = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("timecard_id"));

			if(timecard_id.length())
			{
				if(user.GetType() == "approver")
				{
					// --- check ability to see this timecard
					if(db.Query("SELECT `id` FROM `timecards` WHERE `id`=\"" + timecard_id + "\" AND `contract_sow_id` IN (SELECT `contract_sow_id` FROM `timecard_approvers` WHERE `approver_user_id`=\"" + user.GetID() + "\")"))
					{
						ostResult << "{"
										"\"status\":\"success\","
										"\"timecards\":[" << GetTimecardsInJSONFormat("SELECT * FROM `timecards` WHERE `id`=\"" + timecard_id + "\";", &db, &user, true) << "]"
									"}";
					}
					else
					{
						MESSAGE_ERROR("", action, "user(" + user.GetID() + ") doesn't allow to see this timecard");
						ostResult << "{\"status\":\"error\",\"description\":\"У Вас нет доступа к этой таймкарте\"}";
					}
				}
				else
				{
					MESSAGE_ERROR("", action, "user.id(" + user.GetID() + ") not an approver(" + user.GetType() + ")");
					ostResult << "{\"status\":\"error\",\"description\":\"Вам не назначена роль согласования\"}";
				}
			}
			else
			{
				MESSAGE_ERROR("", action, "parameter timecard_id is empty");
				ostResult << "{\"status\":\"error\",\"description\":\"Некорректые параметры\"}";
			}


			indexPage.RegisterVariableForce("result", ostResult.str());

			if(!indexPage.SetTemplate(template_name))
			{
				MESSAGE_DEBUG("", action, "can't find template " + template_name);
			} // if(!indexPage.SetTemplate("my_network.htmlt"))
		}


		{
			MESSAGE_DEBUG("", action, "finish");
		}
	}

	if(action == "AJAX_getUserProfile")
	{
		string			strPageToGet, strFriendsOnSinglePage;
		ostringstream	ostResult;

		{
			MESSAGE_DEBUG("", action, "start");
		}

		ostResult.str("");
		if(user.GetLogin() == "Guest")
		{
			{
				MESSAGE_DEBUG("", action, "re-login required");
			}

			ostResult << "{\"result\":\"error\",\"description\":\"re-login required\"}";
		}
		else
		{
			string			template_name = "json_response.htmlt";

			if(user.GetType() == "approver")
			{
				ostResult << "{"
								"\"status\":\"success\","
								"\"users\":[" << GetUserListInJSONFormat("SELECT * FROM `users` WHERE `id`=\"" + user.GetID() + "\";", &db, &user) << "],"
								"\"timecard_approvers\":[" << GetApproversInJSONFormat("SELECT * FROM `timecard_approvers` WHERE `approver_user_id`=\"" + user.GetID() + "\";", &db, &user, INCLUDE_SOW_INFO) << "],"
								"\"bt_approvers\":[" << GetApproversInJSONFormat("SELECT * FROM `bt_approvers` WHERE `approver_user_id`=\"" + user.GetID() + "\";", &db, &user, INCLUDE_SOW_INFO) << "]"
							"}";
			}
			else
			{
				MESSAGE_ERROR("", action, "user.id(" + user.GetID() + ") is not an approver(" + user.GetType() + ")");
				ostResult << "{\"status\":\"error\",\"description\":\"Некорректые параметры\"}";
			}

			indexPage.RegisterVariableForce("result", ostResult.str());

			if(!indexPage.SetTemplate(template_name))
			{
				MESSAGE_DEBUG("", action, "can't find template " + template_name);
			} // if(!indexPage.SetTemplate("my_network.htmlt"))
		}

		{
			MESSAGE_DEBUG("", action, "finish");
		}
	}

	if((action == "AJAX_setAutoapprove"))
	{
		string			strPageToGet, strFriendsOnSinglePage;
		ostringstream	ostResult;

		{
			MESSAGE_DEBUG("", action, "start");
		}

		ostResult.str("");
		if(user.GetLogin() == "Guest")
		{
			{
				MESSAGE_DEBUG("", action, "re-login required");
			}

			ostResult << "{\"result\":\"error\",\"description\":\"re-login required\"}";
		}
		else
		{
			string			template_name = "json_response.htmlt";
			string			sow_id = "";
			string			new_state = "";
			string			type = "";

			sow_id = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("sow_id"));
			new_state = CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("new_state"));
			type = CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("type"));

			if((new_state == "Y") || (new_state == "N"))
			{
			}
			else
			{
				new_state = "";
				MESSAGE_ERROR("", action, "unknown request parameter new_state(" + new_state + ")");
			}

			if(sow_id.length() && new_state.length() && ((type == "timecard") || (type == "bt")))
			{
				if(user.GetType() == "approver")
				{
					if(db.Query("SELECT `id`, `auto_approve` FROM `" + type + "_approvers` WHERE `contract_sow_id`=\"" + sow_id + "\" AND `approver_user_id`=\"" + user.GetID() + "\";"))
					{
						string		approver_id = db.Get(0, "id");
						string		curr_state = db.Get(0, "auto_approve");

						if(curr_state == new_state)
						{
							MESSAGE_DEBUG("", action, "user.id/sow.id(" + user.GetID() + "/" + sow_id + ") auto_approve already in correct state");
							ostResult << "{\"result\":\"success\"}";
						}
						else
						{
							db.Query("UPDATE `" + type + "_approvers` SET `auto_approve`=\"" + new_state + "\" WHERE `id`=\"" + approver_id + "\";");
							if(db.isError())
							{
								MESSAGE_ERROR("", action, "fail to update table " + type + " _approvers");
								ostResult << "{\"result\":\"error\",\"description\":\"ошибка БД\"}";
							}
							else
							{
								ostResult << "{\"result\":\"success\"}";
							}
						}
					}
					else
					{
						MESSAGE_ERROR("", action, "user.id(" + user.GetID() + ") doesn't require to approve sow.id(" + sow_id + ")");
						ostResult << "{\"result\":\"error\",\"description\":\"Вашего согласования не требуется\"}";
					}


				}
				else
				{
					MESSAGE_ERROR("", action, "user.id(" + user.GetID() + ") not an approver(" + user.GetType() + ")");
					ostResult << "{\"result\":\"error\",\"description\":\"Вам не назначена роль согласования\"}";
				}
			}
			else
			{
				MESSAGE_ERROR("", action, "any of HTTP-request parameters is empty");
				ostResult << "{\"result\":\"error\",\"description\":\"Некорректые параметры\"}";
			}


			indexPage.RegisterVariableForce("result", ostResult.str());

			if(!indexPage.SetTemplate(template_name))
			{
				MESSAGE_DEBUG("", action, "can't find template " + template_name);
			} // if(!indexPage.SetTemplate("my_network.htmlt"))
		}


		{
			MESSAGE_DEBUG("", action, "finish");
		}
	}

	{
		MESSAGE_DEBUG("", "", "post-condition if(action == \"" + action + "\")");
	}

	indexPage.OutTemplate();

	}
/*
	catch(CExceptionRedirect &c) {
		{
			MESSAGE_DEBUG("", "", "catch CRedirectHTML: exception used for redirection");
		}

		c.SetDB(&db);

		if(!indexPage.SetTemplate(c.GetTemplate()))
		{
			MESSAGE_ERROR("", "", "catch CRedirectHTML: ERROR, template redirect.htmlt not found");
			throw CException("Template file was missing");
		}

		indexPage.RegisterVariableForce("content", "redirect page");
		indexPage.OutTemplate();

	}
*/
	catch(CExceptionHTML &c)
	{

		c.SetLanguage(indexPage.GetLanguage());
		c.SetDB(&db);
		MESSAGE_DEBUG("", action, "catch CExceptionHTML: DEBUG exception reason: [" + c.GetReason() + "]");

		if(!indexPage.SetTemplate(c.GetTemplate()))
		{
			return(-1);
		}
		indexPage.RegisterVariable("content", c.GetReason());
		indexPage.OutTemplate();
		return(0);
	}
	catch(CException &c)
	{
		if(!indexPage.SetTemplateFile("templates/error.htmlt"))
		{
			return(-1);
		}
		{
			MESSAGE_ERROR("", action, "catch CException: exception: ERROR  " + c.GetReason());
		}

		indexPage.RegisterVariable("content", c.GetReason());
		indexPage.OutTemplate();
		return(-1);
	}
	catch(exception& e)
	{
		{
			MESSAGE_ERROR("", action, "catch(exception& e): catch standard exception: ERROR  " + e.what());
		}

		if(!indexPage.SetTemplateFile("templates/error.htmlt"))
		{
			return(-1);
		}
		indexPage.RegisterVariable("content", e.what());
		indexPage.OutTemplate();
		return(-1);
	}

	return(0);
}
