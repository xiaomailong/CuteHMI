import qbs

import cutehmi

import "Test.qbs" as Test

Project {
	Test {
		testName: "test_logging"

		files: [
			"test_logging.cpp",
		]
	}

	Test {
		testName: "test_Exception"

		files: [
			"test_Exception.cpp",
		]

		Depends { name: "Qt.concurrent" }
	}

	Test {
		testName: "test_ExceptionMixin"

		files: [
			"test_ExceptionMixin.cpp",
		]

		Depends { name: "Qt.concurrent" }
	}

	Test {
		testName: "test_Dialog"

		files: [
			"test_Dialog.cpp",
		]
	}

	Test {
		testName: "test_Dialogist"

		files: [
			"test_Dialogist.cpp",
		]
	}

	Test {
		testName: "test_Singleton"

		files: [
			"test_Singleton.cpp",
		]
	}

	Test {
		testName: "test_QML"

		files: [
			"test_QML.cpp",
			"tst_Dialogist.qml",
		]

		Depends { name: "Qt.quick" }
	}
}

//(c)MP: Copyright © 2019, Michal Policht. All rights reserved.
//(c)MP: This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
