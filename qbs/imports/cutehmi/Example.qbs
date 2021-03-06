import qbs
import qbs.FileInfo

import "CommonProduct.qbs" as CommonProduct

CommonProduct {
	type: []

	cutehmiType: "example"

	baseName: name

	condition: project.buildExamples

	cpp.includePaths: [cutehmi.dirs.externalIncludeDir]

	cpp.libraryPaths: [cutehmi.dirs.externalLibDir]

	property string installDir: cutehmi.dirs.examplesInstallDirname + "/" + FileInfo.baseName(sourceDirectory)

	FileTagger {
		patterns: "*.qml"
		fileTags: ["qml"]
	}

	FileTagger {
		patterns: "*.js"
		fileTags: ["js"]
	}

	FileTagger {
		patterns: "*.xml"
		fileTags: ["xml"]
	}

	FileTagger {
		patterns: "*.png"
		fileTags: ["png"]
	}

	FileTagger {
		patterns: "*.svg"
		fileTags: ["svg"]
	}

	Group {
		name: "Project files"
		fileTagsFilter: ["qml", "js", "xml", "png", "svg"]
		qbs.install: true
		qbs.installSourceBase: sourceDirectory
		qbs.installDir: installDir
	}

	Depends { name: "cutehmi.dirs" }
}

//(c)MP: Copyright © 2019, Michal Policht. All rights reserved.
//(c)MP: This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
