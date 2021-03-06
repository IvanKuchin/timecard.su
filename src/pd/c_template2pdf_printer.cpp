#include "c_template2pdf_printer.h"

auto	C_Template2PDF_Printer::RenderTemplate() -> string
{
	auto			error_message = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(GetTemplate().length())
	{
		CCgi		render_obj;
		CVars		vars_obj = vars->GetVars();

		render_obj.SetVars(vars_obj);

		if(render_obj.SetTemplateFile(GetTemplate()))
		{
			content = render_obj.RenderTemplate();

			// --- empty vars error message
			{
				auto	empty_var_list = render_obj.GetEmptyVarList();

				// --- generate error_message
				if(empty_var_list.size())
				{
					auto	empty_var_list_str = ""s;

					for(auto &var_name: empty_var_list)
					{
						if(empty_var_list_str.length()) empty_var_list_str += ", ";
						empty_var_list_str += var_name;
					}

					error_message = gettext("agreement generation failed") + "("s + vars->Get("subcontractor_company_name_") + "). " + gettext("empty vars list") + ": "s + empty_var_list_str;
					MESSAGE_ERROR("", "", "following variable must be defined on contract (" + vars->Get("subcontractor_company_name_") + "): " + empty_var_list_str);
				}

			}
		}
		else
		{
			error_message = gettext("template not found");
			MESSAGE_ERROR("", "", error_message);
		}

		if(error_message.empty())
		{
			if((error_message = SaveTemporaryFile()).length()) { MESSAGE_ERROR("", "", "fail to save file "); }
		}
		else
		{
			MESSAGE_ERROR("", "", "fail to save tempfile due to prev errors");
		}

	}
	else
	{
		MESSAGE_DEBUG("", "", "filename is empty");
	}

	MESSAGE_DEBUG("", "", "finish (error_message.length() = " + to_string(error_message.length()) + ")");

	return error_message;
}

auto	C_Template2PDF_Printer::SaveTemporaryFile() -> string
{
	auto			error_message = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(content.length())
	{
		if(GetFilename().length())
		{
			ofstream ofs;
			ofs.open (GetFilename() + ".html", std::ofstream::out | std::ofstream::app);

			ofs << content;

			ofs.close();
		}
		else
		{
			error_message = gettext("1c filename is empty");
			MESSAGE_ERROR("", "", error_message);
		}
	}
	else
	{
		MESSAGE_DEBUG("", "", "content is empty");
	}


	MESSAGE_DEBUG("", "", "finish (error_message.length() = " + to_string(error_message.length()) + ")");

	return error_message;
}

ostream& operator<<(ostream& os, const C_Template2PDF_Printer &var)
{
	os << "object C_Template2PDF_Printer [empty for now]";

	return os;
}



