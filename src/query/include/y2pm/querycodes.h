/*
   querycodes.h

   define query codes for query keywords

   !! see query.h for id ranges !!

   Author: kkaempf@suse.de
   Maintainer: kkaempf@suse.de

 */
#ifndef QUERYCODES_H
#define QUERYCODES_H

// compare opts, id 1000...
#define QCODE_LIKE		1000
#define QCODE_CONTAINS	1001

// flags, 1..99
#define QCODE_ISINSTALLED	1
#define QCODE_ISAVAILABLE	2
#define QCODE_TOINSTALL	3
#define QCODE_TODELETE	4
#define QCODE_TOUPDATE	5
#define QCODE_ISTABOO	6
#define QCODE_ISAUTOMATIC	7
#define QCODE_ISINCOMPLETE	8
#define QCODE_ISCANDIDATE	9

// values, 100..999
#define QCODE_VERSION		100
#define QCODE_VENDOR		101
#define QCODE_GROUP		102
#define QCODE_NAME		103
#define QCODE_REQUIRES		104
#define QCODE_PROVIDES		105
#define QCODE_CONFLICTS		106
#define QCODE_INSTALLDATE	107
#define QCODE_BUILDDATE		108
#define QCODE_SELECTION		110
#define QCODE_PATCH		111
#define QCODE_FILES		112
#define QCODE_MIME		113
#define QCODE_KEYWORDS		114
#define QCODE_SUPPORTS		115
#define QCODE_NEEDS		116
#define QCODE_SUMMARY		117
#define QCODE_DESCRIPTION	118
#define QCODE_FROM		119		// InstSrc
#define QCODE_IVERSION		120
#define QCODE_AVERSION		121
#define QCODE_CVERSION		122
#define QCODE_EMPTY		123

#endif //QUERYCODES_H

