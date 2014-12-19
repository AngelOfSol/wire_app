#include "command_line_arguments.h"


namespace rmg_com
{

command_line_arguments::command_line_arguments(void)
{
}


command_line_arguments::~command_line_arguments(void)
{
}

command_line_arguments make_args(int argc, char** argv, bool skipFirst)
{
	command_line_arguments ret;
	// check to see if they want to keep the first argument or not
	int i = skipFirst ? 1 : 0;


	// combine all the command line arguments into one string

	enum
	{
		NONE,
		OPTION
	} state = NONE;

	std::string option = "";

	for (; i < argc; i++)
	{
		auto input = std::string(argv[i]);

		if (input.size() > 0)
		{
			if (input[0] == '-')
			{
				option = input.substr(1);
				ret.m_options[option] = "";
				state = OPTION;
			}
			else
			{
				if (state == NONE)
				{
					ret.m_arguments.push_back(input);
				}
				else if (state == OPTION)
				{
					ret.m_options[option] = input;
					state = NONE;
				}
			}
		}
	}

	return ret;
}
//
//command_line_arguments make_args(int argv, char** argc, bool skipFirst)
//{
//	// check to see if they want to keep the first argument or not
//	int i = skipFirst ? 1 : 0;
//
//
//	// combine all the command line arguments into one string
//
//	std::string input = "";
//
//	for(; i < argv; i++)
//	{
//		input += argc[i];
//		input += " ";
//		
//	}
//	
//	return make_args(input);
//
//}
//
//command_line_arguments make_args(const std::string& input)
//{
//	
//	command_line_arguments ret;
//
//	// set the raw input 
//
//	ret.m_raw = input;
//
//	// create states
//	// starting off in state_new
//	enum
//	{
//		STATE_NEW,
//		STATE_OPTION
//	} state = STATE_NEW;
//
//	// for saving option names and values
//	std::string optionName;
//	std::string argumentValue;
//
//	// iterates through a string to make a token out of all characters till next space
//	auto nextToken = [] (std::string::const_iterator& iter, const std::string& container) -> std::string
//	{
//		std::string ret = "";
//		auto ender = ' ';
//		if (*iter == '"')
//			ender = '"';
//		while(*iter != ender && iter != container.end())
//		{
//			ret += *iter;
//			iter++;
//		}
//		return ret;
//	};
//
//	// go through each character in the string
//	for(auto iter = input.begin(); iter != input.end();)
//	{
//		// we're looking at at either a new option or new argument
//		if(state == STATE_NEW)
//		{
//			// its a new option
//			if(*iter == '-')
//			{
//				// move on
//				iter++;
//				// check to see if its a long option or not
//				if(*iter == '-')
//				{
//					// it is, so increment pointer and grab the next whole oken
//					iter++;
//					optionName = nextToken(iter, input);
//				} else
//				{
//					// it isn't so set the option name to the current character
//					optionName = *iter;
//				}
//				// move to the next character that wants to be read
//				iter++;
//				// move on to the option state
//				state = STATE_OPTION;
//			} else
//			{
//				// if it isn't an option its an argument
//				// combine all letters following until a space is found or the end of string is found
//				argumentValue = nextToken(iter, input);
//				
//				ret.m_arguments.push_back(argumentValue);
//				// iterate past the space
//				iter++;
//			}
//		} else if(state == STATE_OPTION)
//		{
//			// we're currently in an option
//			// if the first character is a space, skip it
//			if(*iter == ' ')
//			{
//				iter++;
//				argumentValue = "";
//			} else if(*iter != '-')
//			{
//				// then, if its not another dash grab the option value
//				argumentValue = nextToken(iter, input);
//				iter++;
//			}
//			// otherwise its a blank option
//			// set the option name to have the value
//			ret.m_options[optionName] = argumentValue;
//			// return back state_new
//			state = STATE_NEW;
//		}
//	}
//
//	return ret;
//}

}