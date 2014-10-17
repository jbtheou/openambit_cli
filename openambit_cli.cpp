/*
 * (C) Copyright 2014 Jean-Baptiste Theou
 *
 *
 */
#include "openambit_cli.h"
#define DATA_PATH ""

static struct option long_options[] = {
	{"battery",	no_argument,		0, 'b'},
	{"model",	no_argument,		0, 'm'},
	{"hardware",	no_argument,		0, 'h'},
	{"firmware",	no_argument,		0, 'f'},
	{"gpsorbit",	required_argument,	0, 'g'},
	{"settime",	no_argument,		0, 't'},
	{"log",		no_argument,		0, 'l'},
	{0,		0,			0, 0}
};

static int log_skip_cb(void * ambit_object, ambit_log_header_t *log_header)
{
	struct stat buffer;
	ambit_device_info_t info;
	libambit_device_info_get((ambit_object_t *)ambit_object, &info);
	QDateTime dateTime(QDate(log_header->date_time.year, log_header->date_time.month, log_header->date_time.day),
			QTime(log_header->date_time.hour, log_header->date_time.minute, log_header->date_time.msec/1000));
	QString path(QString(DATA_PATH) + "/log_" +  QString(info.serial) + "_" + dateTime.toString("yyyy_MM_dd_hh_mm_ss") + ".dat");
	return !(stat(path.toStdString().c_str(), &buffer) == 0);
}

static void log_progress_cb(void * ambit_object, uint16_t log_count,
				uint16_t log_current,
				uint8_t progress_percent)
{
	std::cout << log_current << "/" << log_count << std::endl;
}

static void push_cb(void * ambit_object, ambit_log_entry_t *log_entry)
{
	MovesCountJSON jsonParser;
	LogStore logstore;
	QByteArray output;
	ambit_device_info_t info;
	ambit_personal_settings_t settings;
	libambit_device_info_get((ambit_object_t *)ambit_object, &info);
	libambit_personal_settings_get((ambit_object_t *)ambit_object, &settings);
	LogEntry *entry = logstore.store(&info, &settings, log_entry);
	jsonParser.generateLogData(entry, output);
	QDateTime dateTime(QDate(log_entry->header.date_time.year, log_entry->header.date_time.month, log_entry->header.date_time.day),
			QTime(log_entry->header.date_time.hour, log_entry->header.date_time.minute, log_entry->header.date_time.msec/1000));
	QFile datfile(logstore.datEntryPath(QString(info.serial),dateTime));
	datfile.open(QIODevice::WriteOnly);
	datfile.write(output);
	datfile.close();
}

int main(int argc, char* argv[]) {
	ambit_object_t *ambit_object;
	ambit_device_info_t info;
	ambit_device_status_t status;
	time_t current_time;
	struct tm *local_time;
	FILE *gps_file;
	struct stat buf;
	uint8_t *gps_data;
	int gps_len;
	int ret;
	int option_index = 0;

	ambit_object = libambit_detect();
	if (ambit_object != NULL) {
		libambit_device_info_get(ambit_object, &info);
		if (libambit_device_supported(ambit_object)) {
			int c = getopt_long (argc, argv, "b",
				long_options, &option_index);
			switch (c) {
			case 'b':
				ret = libambit_device_status_get(ambit_object,
								&status);
				if (ret == 0)
					std::cout << (int)status.charge;
				break;
			case 'm':
				std::cout << info.model;
				break;
			case 'h':
				std::cout << (int)info.hw_version[0]
					<< "."
					<< (int)info.hw_version[1]
					<< "."
					<< (int)(info.hw_version[2]|
					(info.hw_version[3] << 8));
				break;
			case 'f':
				std::cout << (int)info.fw_version[0]
					<< "."
					<< (int)info.fw_version[1]
					<< "."
					<< (int)(info.fw_version[2]|
					(info.fw_version[3] << 8));
				break;
			case 'g':
				gps_file = fopen(optarg, "rb");
				fstat(fileno(gps_file), &buf);
				gps_len = buf.st_size*sizeof(char);
				gps_data = (uint8_t*) malloc(gps_len);
				ret = libambit_gps_orbit_write(ambit_object,
								gps_data,
								gps_len);
				std::cout << (ret ? "Error" : "Up to date");
				fclose(gps_file);
				break;
			case 't':
				current_time = time(NULL);
				local_time = localtime(&current_time);
				ret = libambit_date_time_set(ambit_object,
							local_time);
				std::cout << (ret ? "KO" : "OK");
				break;
			case 'l':
				libambit_log_read(ambit_object,
						log_skip_cb,
						push_cb,
						log_progress_cb,
						ambit_object);
				std::cout << "Done";
				break;
			}
		}
		libambit_close(ambit_object);
	} else {
		std::cout << "Error";
	}
	return 0;
}

