#pragma once

namespace Ylog
{
	namespace Enums
	{
		enum Levels
		{
			TRACE,
			DEBUG,
			VERBOSE,
			INFO,
			WARNING,
			ERROR,
			CRITICAL,
			FATAL,
			LEVEL_COUNT
		};

		enum DatetimeFormats
		{
			Ymd_HMS,
			Ymd_HM,
			Ymd_H,
			Ymd,
			Ym,
			Year,
			FORMAT_COUNT
		};

		enum MessageFormatFlags
		{
			THREAD_ID	= '0',
			TIMESTAMP	= '1',
			LOG_LEVEL	= '2',
			MESSAGE		= '3',
			MESSAGE_FORMAT_COUNT
		};
	}
}