#ifndef H_EXTENSIONS_CUTEHMI_2_INCLUDE_CUTEHMI_ERRORINFO_HPP
#define H_EXTENSIONS_CUTEHMI_2_INCLUDE_CUTEHMI_ERRORINFO_HPP

#include <QString>

#include <typeinfo>

#include "Error.hpp"

namespace cutehmi {

/**
 * %Error info.
 *
 * @see errorInfo()
 */
struct CUTEHMI_API ErrorInfo
{
	int code;				///< %Error code.
	const char * errClass;	///< %Error class.
	QString str;			///< %Error string.

	/**
	 * Register this class as meta type and return its metatype type id.
	 * @return meta type id.
	 */
	static int RegisterMetaType() noexcept;

	/**
	 * %Error info to string.
	 * @return multi-line string containing information extracted from ErrorInfo struct.
	 */
	QString toString() const;
};

/**
 * Get error info. Produces ErrorInfo from instance of Error or its subclass. This function also registers ErrorInfo with
 * ErrorInfo::RegisterMetaType() function.
 * @param err instance of Error class or its subclass.
 * @return error info structure.
 */
template <typename ERR>
ErrorInfo errorInfo(ERR err)
{
	ErrorInfo::RegisterMetaType();
	return ErrorInfo{err.code(), typeid(ERR).name(), err.str()};
}

}

Q_DECLARE_METATYPE(cutehmi::ErrorInfo)

#endif

//(c)MP: Copyright © 2019, Michal Policht. All rights reserved.
//(c)MP: This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
