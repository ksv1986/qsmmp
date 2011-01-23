# Copyright 1999-2010 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: /var/cvsroot/gentoo-x86/media-sound/qsmmp/qsmmp-9999.ebuild,v 1.1 2011/01/13 23:44:36 dexon Exp $

EAPI=2
inherit qt4-r2 git

EGIT_REPO_URI="git://gitorious.org/qsmmp/${PN}.git"
EGIT_BRANCH="qmmp-9999"
EGIT_COMMIT="${EGIT_BRANCH}"

DESCRIPTION="Qsmmp is a audio player based on Qmmp. Qmmp is a Winamp-like player, while Qsmmp aimed to have native qt look."
LICENSE="GPL-2"
SLOT="0"
KEYWORDS="~amd64 ~x86"
IUSE="debug"

RDEPEND="=media-sound/qmmp-9999"

DEPEND="${RDEPEND}"

