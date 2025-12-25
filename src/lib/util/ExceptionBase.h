/*
 *  ExceptionBase.h
 *
 *  Created on: Apr 24, 2018
 *      Author: kai_horstmann
 *
 *  Exception classes which can be thrown throughout this program
 *
 *   This file is part of OpenVarioFront, an electronic variometer display for glider planes
 *   Copyright (C) 2018  Kai Horstmann
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License along
 *   with this program; if not, write to the Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#ifndef SRC_EXCEPTIONBASE_H_
#define SRC_EXCEPTIONBASE_H_

#include <exception>
#include <string>

namespace OevUtil {

class ExceptionBase :public std::exception {
public:
	ExceptionBase(char const *description)
		:description {description}
		{}
	virtual ~ExceptionBase();

	 virtual const char*
	    what() const noexcept override;

private:
	 std::string description;
};

class SDLException :public ExceptionBase {
public:
	SDLException(char const *description)
		:ExceptionBase {description}
		{}
};

class UuidException :public ExceptionBase {
public:
	UuidException(char const *description)
		:ExceptionBase {description}
		{}
};

}; // namespace OevUtil


namespace OevGLES {

class NativeWindowException :public OevUtil::ExceptionBase {
public:
	NativeWindowException(char const *description)
		:ExceptionBase {description}
		{}

};

class EGLException :public OevUtil::ExceptionBase {
public:
	EGLException(char const *description)
		:ExceptionBase {description}
		{}
};

class ShaderException :public OevUtil::ExceptionBase {
public:
	ShaderException(char const *description)
		:ExceptionBase {description}
		{}
};

class ShaderProgramException :public OevUtil::ExceptionBase {
public:
	ShaderProgramException(char const *description)
		:ExceptionBase {description}
		{}
};

class TextureException :public OevUtil::ExceptionBase {
public:
	TextureException(char const *description)
		:ExceptionBase {description}
		{}
};

class PngReaderException :public OevUtil::ExceptionBase {
public:
	PngReaderException(char const *description)
		:ExceptionBase {description}
		{}
};

class JpegReaderException :public OevUtil::ExceptionBase {
public:
	JpegReaderException(char const *description)
		:ExceptionBase {description}
		{}
};

class GLTextRenderException :public OevUtil::ExceptionBase {
public:
	GLTextRenderException(char const *description)
		:ExceptionBase {description}
		{}
};

class GLObjectWrapperException :public OevUtil::ExceptionBase {
public:
	GLObjectWrapperException(char const *description)
		:ExceptionBase {description}
		{}
};

} /* namespace OevGLES */

namespace OevControls {
		
	class ControlsExceptionBase :public OevUtil::ExceptionBase {
	public:
		ControlsExceptionBase(char const *description)
			:ExceptionBase {description}
			{}
	};
		
	class ControlsFatalException :public ControlsExceptionBase {
	public:
		ControlsFatalException(char const *description)
			:ControlsExceptionBase {description}
			{}
	};

class ControlsAppException :public ControlsExceptionBase {
public:
	ControlsAppException(char const *description)
		:ControlsExceptionBase {description}
		{}
};

} // namespace OevControls


#endif /* SRC_EXCEPTIONBASE_H_ */
