#pragma once

namespace IrStd
{
	namespace ServerImpl
	{
		class MimeType
		{
		public:
			static const char* fromFileExtension(const char* const pExtension) noexcept;
		};
	}
}
