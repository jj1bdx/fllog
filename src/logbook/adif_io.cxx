// ----------------------------------------------------------------------------
// Copyright (C) 2014...2018
//              David Freese, W1HKJ
//
// This file is part of fllog
//
// fldigi is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// fldigi is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
// ----------------------------------------------------------------------------

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <string>

#include <FL/Fl.H>
#include <FL/filename.H>
#include <FL/fl_ask.H>

#include "threads.h"
#include "adif_io.h"
#include "config.h"
#include "lgbook.h"
#include "icons.h"
#include "gettext.h"
#include "debug.h"
#include "util.h"
#include "date.h"
#include "logsupport.h"
#include "support.h"
#include "timeops.h"


#ifdef __WOE32__
static const char *szEOL = "\r\n";
#else
static const char *szEOL = "\n";
#endif
static const char *szEOR = "<EOR>";

// These ADIF fields define the ADIF database
FIELD fields[] = {
//  TYPE,          FSIZE,  NAME,              WIDGET
	{FREQ,         12,    "FREQ",             &btnSelectFreq},      // QSO frequency in Mhz
	{CALL,         30,    "CALL",             &btnSelectCall},      // contacted stations CALLSIGN
	{MODE,         20,    "MODE",             &btnSelectMode},      // QSO mode
	{SUBMODE,      20,    "SUBMODE",          &btnSelectMode},      // QSO submode
	{NAME,         80,    "NAME",             &btnSelectName},      // contacted operators NAME
	{QSO_DATE,     8,     "QSO_DATE",         &btnSelectQSOdateOn}, // QSO data
	{QSO_DATE_OFF, 8,     "QSO_DATE_OFF",     &btnSelectQSOdateOff},// QSO data OFF, according to ADIF 2.2.6
	{TIME_OFF,     6,     "TIME_OFF",         &btnSelectTimeOFF},   // HHMM or HHMMSS in UTC
	{TIME_ON,      6,     "TIME_ON",          &btnSelectTimeON},    // HHMM or HHMMSS in UTC
	{QTH,          100,   "QTH",              &btnSelectQth},       // contacted stations city
	{RST_RCVD,     3,     "RST_RCVD",         &btnSelectRSTrcvd},   // received signal report
	{RST_SENT,     3,     "RST_SENT",         &btnSelectRSTsent},   // sent signal report
	{STATE,        20,    "STATE",            &btnSelectState},     // contacted stations STATE
	{VE_PROV,      20,    "VE_PROV",          &btnSelectProvince},  // 2 letter abbreviation for Canadian Province
	{NOTES,        512,   "NOTES",            &btnSelectNotes},     // QSO notes

	{QSLRDATE,     8,     "QSLRDATE",         &btnSelectQSLrcvd},   // QSL received date
	{QSLSDATE,     8,     "QSLSDATE",         &btnSelectQSLsent},   // QSL sent date

	{EQSLRDATE,    8,     "EQSLRDATE",        &btnSelecteQSLrcvd},  // EQSL received date
	{EQSLSDATE,    8,     "EQSLSDATE",        &btnSelecteQSLsent},  // EQSL sent date

	{LOTWRDATE,    8,     "LOTWRDATE",        &btnSelectLOTWrcvd},  // LoTW received date
	{LOTWSDATE,    8,     "LOTWSDATE",        &btnSelectLOTWsent},  // LoTW sent date

	{GRIDSQUARE,   8,     "GRIDSQUARE",       &btnSelectLOC},       // contacted stations Maidenhead Grid Square
	{BAND,         8,     "BAND",             &btnSelectBand},      // QSO band
	{CNTY,         60,    "CNTY",             &btnSelectCNTY},      // secondary political subdivision, ie: county
	{COUNTRY,      60,    "COUNTRY",          &btnSelectCountry},   // contacted stations DXCC entity name
	{CQZ,          8,     "CQZ",              &btnSelectCQZ},       // contacted stations CQ Zone
	{DXCC,         8,     "DXCC",             &btnSelectDXCC},      // contacted stations Country Code
	{QSL_VIA,      256,   "QSL_VIA",          &btnSelectQSL_VIA},   // contacted stations path
	{IOTA,         20,    "IOTA",             &btnSelectIOTA},      // Islands on the air
	{ITUZ,         20,    "ITUZ",             &btnSelectITUZ},      // ITU zone
	{CONT,         60,    "CONT",             &btnSelectCONT},      // contacted stations continent

	{SRX,          50,    "SRX",              &btnSelectSerialIN},  // received serial number for a contest QSO
	{STX,          50,    "STX",              &btnSelectSerialOUT}, // QSO transmitted serial number

	{XCHG1,        100,   "SRX_STRING",       &btnSelectXchgIn},    // contest exchange #1 / free1 in xlog
	{MYXCHG,       100,   "STX_STRING",       &btnSelectMyXchg},    // contest exchange sent

	{CLASS,        20,    "CLASS",            &btnSelectClass},     // Field Day / School RR class received
	{ARRL_SECT,    20,    "ARRL_SECT",        &btnSelectSection},   // ARRL section received

	{TX_PWR,       8,     "TX_PWR",           &btnSelectTX_pwr},    // power transmitted by this station

	{OP_CALL,     30,     "OPERATOR",         &btnSelectOperator},  // Callsign of person logging the QSO
	{STA_CALL,    30,     "STATION_CALLSIGN", &btnSelectStaCall},   // Callsign of transmitting station
	{MY_GRID,      8,     "MY_GRIDSQUARE",    &btnSelectStaGrid},   // Xmt station locator
	{MY_CITY,     60,     "MY_CITY",          &btnSelectStaCity},   // Xmt station location

	{SS_SEC,       20,    "CWSS_SECTION",     &btnSelect_cwss_section},   // CW sweepstakes
	{SS_SERNO,     20,    "CWSS_SERNO",       &btnSelect_cwss_serno},
	{SS_PREC,      20,    "CWSS_PREC",        &btnSelect_cwss_prec},
	{SS_CHK,       20,    "CWSS_CHK",         &btnSelect_cwss_check},

	{AGE,          2,     "AGE",              &btnSelectAge},       // contacted operators age in years
	{TEN_TEN,      10,    "TEN_TEN",          &btnSelect_1010},     // ten ten # of other station
	{CHECK,        10,    "CHECK",            &btnSelectCheck},     // contest identifier

	{FD_CLASS,     20,    "FD_CLASS",         &btnSelectClass},     // Field Day Rcvd
	{FD_SECTION,   20,    "FD_SECTION",       &btnSelectSection},   // FD section received

	{TROOPS,       20,    "TROOPS",           &btnSelectJOTA},      // JOTA troop number sent
	{TROOPR,       20,    "TROOPR",           &btnSelectJOTA},      // JOTA troop number received
	{SCOUTS,       20,    "SCOUTS",           &btnSelectJOTA},
	{SCOUTR,       20,    "SCOUTR",           &btnSelectJOTA},

	{NUMFIELDS,    0,     "",             NULL}
};

// This ADIF fields is in the fldigi QSO database, but not saved in the ADIF file
/*
	{EXPORT,       0,     "EXPORT",       NULL},                // used to indicate record is to be exported
*/

// These ADIF fields are not in the fldigi QSO database
/*
	{COMMENT,      256,   "COMMENT",      NULL},                // comment field for QSO
	{ADDRESS,      256,   "ADDRESS",      NULL},                // contacted stations mailing address
	{CONTEST_ID,   20,    "CONTEST_ID",   NULL},                // QSO contest identifier
	{PFX,          20,    "PFX",          NULL},                // WPA prefix
	{PROP_MODE,    100,   "PROP_MODE",    NULL},                // propogation mode
	{QSL_MSG,      256,   "QSL_MSG",      NULL},                // personal message to appear on qsl card
	{QSL_RCVD,     4,     "QSL_RCVD",     NULL},                // QSL received status
	{QSL_SENT,     4,     "QSL_SENT",     NULL},                // QSL sent status
	{RX_PWR,       8,     "RX_PWR",       NULL},                // power of other station in watts
	{SAT_MODE,     20,    "SAT_MODE",     NULL},                // satellite mode
	{SAT_NAME,     20,    "SAT_NAME",     NULL},                // satellite name
};
*/

static std::string read_errors;
static int    num_read_errors;

static char *fastlookup = 0;

static unsigned int maxlen = 0;

static void initfields()
{
	if (fastlookup) return; // may have multiple instances using common code
	int i = 0;
	while (fields[i].type != NUMFIELDS) {
		if (strlen(fields[i].name) > maxlen) maxlen = strlen(fields[i].name);
		i++;
	}
	maxlen++;
	fastlookup = new char[maxlen * i + 1];
	fastlookup[0] = 0;
	i = 0;
	while (fields[i].type != NUMFIELDS) {
		strcat(fastlookup, fields[i].name);
		unsigned int n = maxlen - strlen(fastlookup) % maxlen;
		if (n > 0 && n < maxlen) for (unsigned int j = 0; j < n; j++) strcat(fastlookup, " ");
		i++;
	}
}

static inline int findfield( char *p )
{
	if (strncasecmp (p, "EOR>", 4) == 0 || !maxlen)
		return -1;

	char *pos;
	char *p1 = strchr(p, ':');
	char *p2 = strchr(p, '>');
	if (p1 && p2) {
		if (p1 < p2) {
			pos = p;
			do { *pos = toupper(*pos); } while (++pos < p1);
			*p1 = 0;
			pos = strcasestr(fastlookup, p);
			*p1 = ':';
			if (pos) {
				return fields[(pos - fastlookup) / maxlen].type;
			}
		}
	}
	return -2;		//search key not found
}

int cAdifIO::instances = 0;

cAdifIO::cAdifIO ()
{
	initfields();
	instances++;
}

cAdifIO::~cAdifIO()
{
	if (--instances == 0) {
		delete [] fastlookup;
		fastlookup = 0;
	}
}

char * cAdifIO::fillfield (int fieldnum, char *buff, int recnbr)
{
	char *p1 = strchr(buff, ':');
	char *p2 = strchr(buff, '>');
	if (!p1 || !p2 || p2 < p1) {
		return 0; // bad ADIF specifier ---> no ':' after field name
	}

	p1++;
	int fldsize = 0;
	while (p1 != p2) {
		if (*p1 >= '0' && *p1 <= '9') {
			fldsize = fldsize * 10 + *p1 - '0';
		}
		p1++;
	}

	std::string tmp = "";
	tmp.assign(p2+1, fldsize);

// added to disallow very large corrupted adif fields
	if (fldsize > fields[fieldnum].fsize) {
		std::string bfr = buff;
		tmp.erase(fields[fieldnum].fsize);
		if (recnbr != -1) {
			static char szmsg[1000];
			snprintf(szmsg, sizeof(szmsg), 
				"In record # %d, <%s, too large, saving first %d characters\n", 
				recnbr+1,
				bfr.substr(0, (int)(p2+1 - buff)).c_str(),
				fields[fieldnum].fsize );
			read_errors.append(szmsg);
		}
		num_read_errors++;
	}

	if ((fieldnum == TIME_ON || fieldnum == TIME_OFF) && fldsize < 6)
		while (tmp.length() < 6) tmp += '0';

	adifqso->putField( fieldnum, tmp.c_str(), tmp.length() );

	return p2 + fldsize + 1;
}

void cAdifIO::readfile_(std::string fname, cQsoDb *db)
{
	int found;
	static char szmsg[500];

	read_errors.clear();
	num_read_errors = 0;

// open the adif file
	FILE *adiFile = fl_fopen (fname.c_str(), "rb");

	if (adiFile == NULL) {
		LOG_ERROR("Could not open %s", fname.c_str());
		return;
	}

	struct timespec t0, t1;
#ifdef _POSIX_MONOTONIC_CLOCK
	clock_gettime(CLOCK_MONOTONIC, &t0);
#else
	clock_gettime(CLOCK_REALTIME, &t0);
#endif

	char buff[16384];
	std::string sbuff;
	memset(buff, 0, 16384);
	int retnbr = fread(buff, 1, 16384, adiFile);
	if (retnbr) sbuff.assign(buff, retnbr);

	size_t p = sbuff.find("<EOH>");
	if (p == std::string::npos) p = sbuff.find("<eoh>");
	if (p == std::string::npos) {
		return;
	}

	sbuff.erase(0, p + 5);
// skip over cr/lf pairs
	while (sbuff.length() && (sbuff[0] == '\r' || sbuff[0] == '\n'))
		sbuff.erase(0,1);

	p = sbuff.find("<EOR>");
	if (p == std::string::npos) p = sbuff.find("<eor>");

	while (!feof(adiFile) && (p == std::string::npos)) {
		retnbr = fread(buff, 1, 16384, adiFile);
		if (retnbr) sbuff.append(buff, retnbr);
		p = sbuff.find("<EOR>");
		if (p == std::string::npos) p = sbuff.find("<eor>");
	}
	if (p == std::string::npos) return;

	char *ptr = 0, *ptr2 = 0;
	int recnbr = 0;

	while (p != std::string::npos) {

		ptr = strchr((char *)sbuff.c_str(),'<');

		adifqso = 0;
		while (ptr) {
			found = findfield(ptr+1);
			if (found > -1) {
				if (!adifqso) adifqso = db->newrec(); // need new record in db
				ptr2 = fillfield (found, ptr+1, recnbr );
			} else if (found == -1) { // <eor> reached;
				break;
			} else {
				ptr2 = ptr + 1;
			}
			if ((ptr2 != NULL) && (unsigned)(ptr2 - ptr) <= p)
				ptr = strchr(ptr2,'<');
			else
				break; // corrupt record
		}
		recnbr++;

		sbuff.erase(0, p + 5);
		while (sbuff.length() && (sbuff[0] == '\r' || sbuff[0] == '\n'))
			sbuff.erase(0,1);

		p = sbuff.find("<EOR>");
		if (p == std::string::npos) p = sbuff.find("<eor>");

		while (!feof(adiFile) && (p == std::string::npos)) {
			retnbr = fread(buff, 1, 16384, adiFile);
			if (retnbr) sbuff.append(buff, retnbr);
			p = sbuff.find("<EOR>");
			if (p == std::string::npos) p = sbuff.find("<eor>");
		}
	}

#ifdef _POSIX_MONOTONIC_CLOCK
	clock_gettime(CLOCK_MONOTONIC, &t1);
#else
	clock_gettime(CLOCK_REALTIME, &t1);
#endif

	float t = t1.tv_sec - t0.tv_sec + (t1.tv_nsec - t0.tv_nsec)/1e9;


	if (!feof(adiFile)) {
		snprintf(szmsg, sizeof(szmsg), "\
================================================\n\
ERROR reading logbook %s\n\
      read %d records in %4.1f seconds\n\
================================================", fname.c_str(), db->nbrRecs(), t);
		read_errors.assign(szmsg);
	} else {
		snprintf(szmsg, sizeof(szmsg), "\
================================================\n\
Read Logbook: %s\n\
      read %d records in %4.1f seconds\n\
================================================", fname.c_str(), db->nbrRecs(), t);
		read_errors.assign(szmsg);
		if (num_read_errors) {
			snprintf(szmsg, sizeof(szmsg), "\
\nCorrected %d errors.\n\
================================================", num_read_errors);
			read_errors.append(szmsg);
		}
	}
	LOG_INFO("%s", read_errors.c_str());

	fclose(adiFile);
}

static const char *adifmt = "<%s:%d>";

// write ALL or SELECTED records to the designated file

int cAdifIO::writeFile (std::string fname, cQsoDb *db)
{
	std::string ADIFHEADER;
	ADIFHEADER = "File: %s";
	ADIFHEADER.append(szEOL);
	ADIFHEADER.append("<ADIF_VER:%d>%s");
	ADIFHEADER.append(szEOL);
	ADIFHEADER.append("<PROGRAMID:%d>%s");
	ADIFHEADER.append(szEOL);
	ADIFHEADER.append("<PROGRAMVERSION:%d>%s");
	ADIFHEADER.append(szEOL);
	ADIFHEADER.append("<EOH>");
	ADIFHEADER.append(szEOL);
// open the adif file
	cQsoRec *rec;
	std::string sFld;
	adiFile = fl_fopen (fname.c_str(), "wb");
	if (!adiFile)
		return 1;

	fprintf (adiFile, ADIFHEADER.c_str(),
			 fl_filename_name(fname.c_str()),
			 strlen(ADIF_VERS), ADIF_VERS,
			 strlen(PACKAGE_NAME), PACKAGE_NAME,
			 strlen(PACKAGE_VERSION), PACKAGE_VERSION);

	for (int i = 0; i < db->nbrRecs(); i++) {
		rec = db->getRec(i);
		if (rec->getField(EXPORT)[0] == 'E') {
			int j = 0;
			while (fields[j].type != NUMFIELDS) {
				if (strcmp(fields[j].name,"MYXCHG") == 0) { j++; continue; }
				if (strcmp(fields[j].name,"XCHG1") == 0) { j++; continue; }
				if (fields[j].btn != NULL)
					if ((*fields[j].btn)->value()) {
						sFld = rec->getField(fields[j].type);
						if (!sFld.empty()) {
							fprintf(adiFile, adifmt,
								fields[j].name,//->c_str(),
								sFld.length());
							fprintf(adiFile, "%s", sFld.c_str());
						}
				}
				j++;
			}
			rec->putField(EXPORT,"");
			db->qsoUpdRec(i, rec);
			fprintf(adiFile, "%s", szEOR);
			fprintf(adiFile, "%s", szEOL);
		}
	}
	fclose (adiFile);

	return 0;
}

// write ALL records to the common log

static std::string adif_file_image;
static std::string adif_file_name;
static std::string records;
static std::string record;
static char recfield[200];
static int nrecs;

static cQsoDb *adif_db;

static cAdifIO *adifIO = 0;

void cAdifIO::readFile (std::string fname, cQsoDb *db)
{
	adif_file_name = fname.c_str();
	adif_db = db;
	adifIO = this;
	readfile_(fname, adif_db);
}

static cQsoDb *adifdb = 0;
static cQsoDb *wrdb = 0;

static struct timespec t0, t1;

std::string cAdifIO::adif_record(cQsoRec *rec)
{
	static std::string record;
	static std::string sFld;
	record.clear();
	for (int j = 0; fields[j].type != NUMFIELDS; j++) {
		if (strcmp(fields[j].name,"MYXCHG") == 0) continue;
		if (strcmp(fields[j].name,"XCHG1") == 0) continue;
		sFld = rec->getField(fields[j].type);
		if (!sFld.empty()) {
			snprintf(recfield, sizeof(recfield),
				adifmt,
				fields[j].name,
				sFld.length());
			record.append(recfield).append(sFld);
		}
	}
	record.append(szEOR);
	record.append(szEOL);
	return record;
}

int cAdifIO::writeAdifRec (cQsoRec *rec, const char *fname)
{
	std::string strRecord = adif_record(rec);

	FILE *adiFile = fl_fopen (fname, "ab");

	if (!adiFile) {
		LOG_ERROR("Cannot write to %s", fname);
		return 1;
	}
	LOG_INFO("Write record to %s", fname);

	fprintf (adiFile, "%s", strRecord.c_str());

	fclose (adiFile);

	return 0;
}

int cAdifIO::writeLog (const char *fname, cQsoDb *db, bool immediate) {

#ifdef _POSIX_MONOTONIC_CLOCK
	clock_gettime(CLOCK_MONOTONIC, &t0);
#else
	clock_gettime(CLOCK_REALTIME, &t0);
#endif

	if (!immediate) {
		adif_file_name = fname;
		adifIO = this;
		if (wrdb) delete wrdb;
		wrdb = new cQsoDb(db);
		adifdb = wrdb;
	} else {
		adif_file_name = fname;
		adifdb = db;
		writelog_();
	}
	return 1;
}

void cAdifIO::writelog_()
{
	std::string ADIFHEADER;
	ADIFHEADER = "File: %s";
	ADIFHEADER.append(szEOL);
	ADIFHEADER.append("<ADIF_VER:%d>%s");
	ADIFHEADER.append(szEOL);
	ADIFHEADER.append("<PROGRAMID:%d>%s");
	ADIFHEADER.append(szEOL);
	ADIFHEADER.append("<PROGRAMVERSION:%d>%s");
	ADIFHEADER.append(szEOL);
	ADIFHEADER.append("<EOH>");
	ADIFHEADER.append(szEOL);

	adiFile = fl_fopen (adif_file_name.c_str(), "wb");

	if (!adiFile) {
		LOG_ERROR("Cannot write to %s", adif_file_name.c_str());
		if (wrdb) delete wrdb;
		return;
	}
	LOG_INFO("Writing %s", adif_file_name.c_str());

	cQsoRec *rec;

	records.clear();
	for (int i = 0; i < adifdb->nbrRecs(); i++) {
		rec = adifdb->getRec(i);
		records.append(adif_record(rec));
		adifdb->qsoUpdRec(i, rec);
	}
	nrecs = adifdb->nbrRecs();

	fprintf (adiFile, ADIFHEADER.c_str(),
		 fl_filename_name(adif_file_name.c_str()),
		 strlen(ADIF_VERS), ADIF_VERS,
		 strlen(PACKAGE_NAME), PACKAGE_NAME,
		 strlen(PACKAGE_VERSION), PACKAGE_VERSION
		);
	fprintf (adiFile, "%s", records.c_str());

	fclose (adiFile);

	if (wrdb) delete wrdb;

#ifdef _POSIX_MONOTONIC_CLOCK
	clock_gettime(CLOCK_MONOTONIC, &t1);
#else
	clock_gettime(CLOCK_REALTIME, &t1);
#endif

	t0 = t1 - t0;
	float t = (t0.tv_sec + t0.tv_nsec/1e9);

	static char szmsg[50];
	snprintf(szmsg, sizeof(szmsg), "%d records in %4.2f seconds", adifdb->nbrRecs(), t);
	LOG_INFO("%s", szmsg);

//	snprintf(szmsg, sizeof(szmsg), "Wrote log %d recs", adifdb->nbrRecs());
//	put_status(szmsg, 5.0);

	return;
}

// ---------------------------------------------------------------------
// update_record
//
//   used by xmlrpc server to update the current record in the database
//   buffer contains ADIF fields with <eor> as terminator
//   db points to the current database
//
//----------------------------------------------------------------------
void cAdifIO::update_record(const char *buffer, cQsoDb &db)
{
	int found;
	char * p = strchr((char *)buffer,'<'); // find first ADIF specifier

	int editnbr = wBrowser->value();

	adifqso = qsodb.getRec (editnbr);
	if (!adifqso) return;

	while (p) {
		found = findfield(p + 1);
		if (found > -1) {
			fillfield (found, p + 1);
		} else if (found == -1) // <eor> reached;
			break;
		p = strchr(p + 1,'<');
	}

	db.qsoUpdRec (editnbr, adifqso);

	db.isdirty(0);
	db.SortByDate();

	std::string st = "Updated record: ";
	st.append(buffer);
	LOG_INFO("%s", st.c_str());

	writeLog(progStatus.logbookfilename.c_str(), &db, true);

	loadBrowser(true);

}

// ---------------------------------------------------------------------
// add_record
//
// used by xmlrpc server to add a record to the database
//   buffer contains an ADIF record with <eor> as terminator
//   db points to the current database
//
//----------------------------------------------------------------------
void cAdifIO::add_record(const char *buffer, cQsoDb &db)
{
	int found;
	char * p = strchr((char *)buffer,'<'); // find first ADIF specifier

	adifqso = db.newrec();
	while (p) {
		found = findfield(p + 1);
		if (found > -1)
			fillfield (found, p + 1);
		else if (found == -1) // <eor> reached;
			break;
		p = strchr(p + 1,'<');
	}
	std::string st = buffer;

	if (st.find("<OP_CALL:") == std::string::npos)
		fillfield(OP_CALL, const_cast<char *>(progStatus.opcall.c_str()));
	if (st.find("<STA_CALL:") == std::string::npos)
		fillfield(STA_CALL, const_cast<char *>(progStatus.mycall.c_str()));

	db.SortByDate();

	size_t ptr = st.find("<CALL:");
	if (ptr != std::string::npos) ptr = st.find(">", ptr);
	st.erase(0, ptr + 1);
	ptr = st.find("<");
	st.erase(ptr);
	st.insert(0, "Add record: ");
	LOG_INFO("%s", st.c_str());

	writeLog(progStatus.logbookfilename.c_str(), &db, true);

}
