// This file is part of Notepad++ project
// Copyright (C)2025 Don HO <don.h@free.fr>

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// at your option any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#define    IDM    40000

#define    IDM_FILE    (IDM + 1000)
#define    IDM_FILE_NEW                              (IDM_FILE + 1)
#define    IDM_FILE_OPEN                             (IDM_FILE + 2)
#define    IDM_FILE_CLOSE                            (IDM_FILE + 3)
#define    IDM_FILE_CLOSEALL                         (IDM_FILE + 4)
#define    IDM_FILE_CLOSEALL_BUT_CURRENT             (IDM_FILE + 5)
#define    IDM_FILE_SAVE                             (IDM_FILE + 6)
#define    IDM_FILE_SAVEALL                          (IDM_FILE + 7)
#define    IDM_FILE_SAVEAS                           (IDM_FILE + 8)
#define    IDM_FILE_CLOSEALL_TOLEFT                  (IDM_FILE + 9)
#define    IDM_FILE_PRINT                            (IDM_FILE + 10)
#define    IDM_FILE_PRINTNOW                         1001
#define    IDM_FILE_EXIT                             (IDM_FILE + 11)

#define    IDM_EDIT       (IDM + 2000)
#define    IDM_EDIT_CUT                                     (IDM_EDIT + 1)
#define    IDM_EDIT_COPY                                    (IDM_EDIT + 2)
#define    IDM_EDIT_UNDO                                    (IDM_EDIT + 3)
#define    IDM_EDIT_REDO                                    (IDM_EDIT + 4)
#define    IDM_EDIT_PASTE                                   (IDM_EDIT + 5)
#define    IDM_EDIT_DELETE                                  (IDM_EDIT + 6)
#define    IDM_EDIT_SELECTALL                               (IDM_EDIT + 7)

#define    IDM_SEARCH    (IDM + 3000)
#define    IDM_SEARCH_FIND                 (IDM_SEARCH + 1)
#define    IDM_SEARCH_FINDNEXT             (IDM_SEARCH + 2)
#define    IDM_SEARCH_REPLACE              (IDM_SEARCH + 3)
#define    IDM_SEARCH_GOTOLINE             (IDM_SEARCH + 4)

#define    IDM_VIEW    (IDM + 4000)
#define    IDM_VIEW_ZOOMIN                    (IDM_VIEW + 23)
#define    IDM_VIEW_ZOOMOUT                   (IDM_VIEW + 24)
#define    IDM_VIEW_ZOOMRESTORE               (IDM_VIEW + 33)
