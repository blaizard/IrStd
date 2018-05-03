#include <cstdint>
#include <cstring>
#include <algorithm>
#include <array>

#include <iostream>

#include "MimeType.hpp"

#define STR_TO_UINT64_STATIC(str) STR_TO_UINT64(str "        ")

#define STR_TO_UINT64(str) \
	static_cast<uint64_t>((str)[0]) \
	+ (static_cast<uint64_t>((str)[1]) << 8) \
	+ (static_cast<uint64_t>((str)[2]) << 16) \
	+ (static_cast<uint64_t>((str)[3]) << 24) \
	+ (static_cast<uint64_t>((str)[4]) << 32) \
	+ (static_cast<uint64_t>((str)[5]) << 40) \
	+ (static_cast<uint64_t>((str)[6]) << 48) \
	+ (static_cast<uint64_t>((str)[7]) << 56)

const char* IrStd::ServerImpl::MimeType::fromFileExtension(const char* const pExtension) noexcept
{
	// Convert the extension to a 8 byte (64-bit) number, to speed up comparison
	std::array<char, 9> extensionLower{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '};
	std::memcpy(extensionLower.data(), pExtension, std::min(std::strlen(pExtension), static_cast<size_t>(8)));
	std::transform(extensionLower.begin(), extensionLower.end(), extensionLower.begin(), ::tolower);
	const uint64_t extension = STR_TO_UINT64(extensionLower.data());

	switch (extension)
	{
	case STR_TO_UINT64_STATIC("aac"):
		return "audio/aac";
	case STR_TO_UINT64_STATIC("abw"):
		return "application/x-abiword";
	case STR_TO_UINT64_STATIC("arc"):
		return "application/octet-stream";
	case STR_TO_UINT64_STATIC("avi"):
		return "application/vnd.amazon.ebook";
	case STR_TO_UINT64_STATIC("bin"):
		return "application/octet-stream";
	case STR_TO_UINT64_STATIC("bz"):
		return "application/x-bzip";
	case STR_TO_UINT64_STATIC("bz2"):
		return "application/x-bzip2";
	case STR_TO_UINT64_STATIC("csh"):
		return "application/x-csh";
	case STR_TO_UINT64_STATIC("css"):
		return "text/css";
	case STR_TO_UINT64_STATIC("csv"):
		return "text/csv";
	case STR_TO_UINT64_STATIC("doc"):
		return "application/msword";
	case STR_TO_UINT64_STATIC("eot"):
		return "application/vnd.ms-fontobject";
	case STR_TO_UINT64_STATIC("epub"):
		return "application/epub+zip";
	case STR_TO_UINT64_STATIC("gif"):
		return "image/gif";
	case STR_TO_UINT64_STATIC("htm"):
	case STR_TO_UINT64_STATIC("html"):
		return "text/html";
	case STR_TO_UINT64_STATIC("ico"):
		return "image/x-icon";
	case STR_TO_UINT64_STATIC("ics"):
		return "text/calendar";
	case STR_TO_UINT64_STATIC("jar"):
		return "application/java-archive";
	case STR_TO_UINT64_STATIC("jpg"):
	case STR_TO_UINT64_STATIC("jpeg"):
		return "image/jpeg";
	case STR_TO_UINT64_STATIC("js"):
		return "application/javascript";
	case STR_TO_UINT64_STATIC("json"):
		return "application/json";
	case STR_TO_UINT64_STATIC("mid"):
	case STR_TO_UINT64_STATIC("midi"):
		return "audio/midi";
	case STR_TO_UINT64_STATIC("mpeg"):
		return "video/mpeg";
	case STR_TO_UINT64_STATIC("mpkg"):
		return "application/vnd.apple.installer+xml";
	case STR_TO_UINT64_STATIC("odp"):
		return "application/vnd.oasis.opendocument.presentation";
	case STR_TO_UINT64_STATIC("ods"):
		return "application/vnd.oasis.opendocument.spreadsheet";
	case STR_TO_UINT64_STATIC("odt"):
		return "application/vnd.oasis.opendocument.text";
	case STR_TO_UINT64_STATIC("oga"):
		return "audio/ogg";
	case STR_TO_UINT64_STATIC("ogv"):
		return "video/ogg";
	case STR_TO_UINT64_STATIC("ogx"):
		return "application/ogg";
	case STR_TO_UINT64_STATIC("otf"):
		return "font/otf";
	case STR_TO_UINT64_STATIC("png"):
		return "image/png";
	case STR_TO_UINT64_STATIC("pdf"):
		return "application/pdf";
	case STR_TO_UINT64_STATIC("ppt"):
		return "application/vnd.ms-powerpoint";
	case STR_TO_UINT64_STATIC("rar"):
		return "application/x-rar-compressed";
	case STR_TO_UINT64_STATIC("rtf"):
		return "application/rtf";
	case STR_TO_UINT64_STATIC("sh"):
		return "application/x-sh";
	case STR_TO_UINT64_STATIC("svg"):
		return "image/svg+xml";
	case STR_TO_UINT64_STATIC("swf"):
		return "application/x-shockwave-flash";
	case STR_TO_UINT64_STATIC("tar"):
		return "application/x-tar";
	case STR_TO_UINT64_STATIC("tif"):
	case STR_TO_UINT64_STATIC("tiff"):
		return "image/tiff";
	case STR_TO_UINT64_STATIC("ts"):
		return "video/vnd.dlna.mpeg-tts";
	case STR_TO_UINT64_STATIC("ttf"):
		return "font/ttf";
	case STR_TO_UINT64_STATIC("vsd"):
		return "application/vnd.visio";
	case STR_TO_UINT64_STATIC("wav"):
		return "audio/x-wav";
	case STR_TO_UINT64_STATIC("weba"):
		return "audio/webm";
	case STR_TO_UINT64_STATIC("webm"):
		return "video/webm";
	case STR_TO_UINT64_STATIC("webp"):
		return "image/webp";
	case STR_TO_UINT64_STATIC("woff"):
		return "font/woff";
	case STR_TO_UINT64_STATIC("woff2"):
		return "font/woff2";
	case STR_TO_UINT64_STATIC("xhtml"):
		return "application/xhtml+xml";
	case STR_TO_UINT64_STATIC("xls"):
		return "application/vnd.ms-excel";
	case STR_TO_UINT64_STATIC("xml"):
		return "application/xml";
	case STR_TO_UINT64_STATIC("xul"):
		return "application/vnd.mozilla.xul+xml";
	case STR_TO_UINT64_STATIC("zip"):
		return "application/zip";
	case STR_TO_UINT64_STATIC("3gp"):
		return "video/3gpp";
	case STR_TO_UINT64_STATIC("3g2"):
		return "video/3gpp2";
	case STR_TO_UINT64_STATIC("7z"):
		return "application/x-7z-compressed";

	default:
		return "application/octet-stream";
	}	
}


//2314885532098589800 -> 202020206C6D7468

//6C6D7468