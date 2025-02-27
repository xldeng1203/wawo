#ifndef _WAWO_TIME_TIME_HPP_
#define _WAWO_TIME_TIME_HPP_

#include <ctime>

#include <wawo/core.h>
#include <wawo/string.hpp>

#define WAWO_USE_CHRONO
#ifdef WAWO_USE_CHRONO
	#include <chrono>
#endif

namespace wawo { namespace time {

#ifdef WAWO_USE_CHRONO
	using namespace std::chrono;
#endif

	inline void time_of_day( struct timeval &tv, void *tzp ) {

#ifdef WAWO_PLATFORM_WIN

#ifdef WAWO_USE_CHRONO
		std::chrono::time_point<std::chrono::system_clock, std::chrono::microseconds> tp = std::chrono::time_point_cast<std::chrono::microseconds>( std::chrono::system_clock::now() );

		std::chrono::seconds sec = std::chrono::time_point_cast<std::chrono::seconds>(tp).time_since_epoch() ;
		std::chrono::microseconds mic_sec= std::chrono::time_point_cast<std::chrono::microseconds>(tp).time_since_epoch();

		std::chrono::duration<double> du = mic_sec - sec;
		tv.tv_sec = static_cast<long>(sec.count()) ;
		tv.tv_usec = static_cast<long>(du.count()*1000000) ;
#else
		time_t clock;
		struct tm tm;
		SYSTEMTIME wtm;
		GetLocalTime(&wtm);
		tm.tm_year    = wtm.wYear - 1900;
		tm.tm_mon     = wtm.wMonth - 1;
		tm.tm_mday    = wtm.wDay;
		tm.tm_hour    = wtm.wHour;
		tm.tm_min     = wtm.wMinute;
		tm.tm_sec     = wtm.wSecond;
		tm.tm_isdst   = -1;
		clock = mktime(&tm);
		tv.tv_sec = static_cast<long int>(clock) ;
		tv.tv_usec = wtm.wMilliseconds * 1000;
#endif
#elif defined(WAWO_PLATFORM_POSIX)
		gettimeofday(&tv, NULL );
#else
#error
#endif

	}

	inline uint64_t curr_nanoseconds() {
#ifdef WAWO_USE_CHRONO
		std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds> tp = std::chrono::time_point_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now() );
		return tp.time_since_epoch().count();
#else
	#error
#endif
	}

	inline uint64_t curr_microseconds() {
#ifdef WAWO_USE_CHRONO
		std::chrono::time_point<std::chrono::system_clock, std::chrono::microseconds> tp = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::system_clock::now() );
		return tp.time_since_epoch().count();
#else
		struct timeval tv;
		time_of_day( tv, NULL );
		return ((uint64_t) tv.tv_sec)* 1000000 + tv.tv_usec ;
#endif
	}

	inline uint64_t curr_milliseconds() {
#ifdef WAWO_USE_CHRONO
		std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds> tp = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() );
		return tp.time_since_epoch().count();
#else
		struct timeval tv;
		time_of_day( tv, NULL );
		return ((uint64_t) tv.tv_sec)* 1000 ;
#endif
	}

	inline uint64_t curr_seconds() {
#ifdef WAWO_USE_CHRONO
		std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds> tp = std::chrono::time_point_cast<std::chrono::seconds>(std::chrono::system_clock::now() );
		return tp.time_since_epoch().count();
#else
		struct timeval tv;
		time_of_day( tv, NULL );
		return (uint64_t) tv.tv_sec ;
#endif
	}

	inline void to_localtime_str( struct timeval const&tv, Len_CStr& lcstr ) {
		char buf[] = "1970-01-01 00:00:00.000000"; //our time format

		const static char* _fmt_seconds = "%Y-%m-%d %H:%M:%S.000";
		const static char* _fmt_mseconds = "%03d";

		time_t long_time = (time_t) tv.tv_sec;
		struct tm timeinfo;
#ifdef WAWO_PLATFORM_WIN
		localtime_s(&timeinfo,&long_time) ;
#else
		localtime_r(&long_time,&timeinfo);
#endif
		WAWO_ASSERT( strlen(buf) == 26 );
		strftime(buf, sizeof(buf), _fmt_seconds, &timeinfo );
		snprintf( (char*)(&buf[0] + 20), 6, _fmt_mseconds , (int) (tv.tv_usec/1000) );

		lcstr = Len_CStr(buf);
	}

	inline void curr_localtime_str(Len_CStr& lcstr) {
		struct timeval tv ;
		time_of_day(tv,0);
		to_localtime_str(tv,lcstr);
	}

}}
#endif
