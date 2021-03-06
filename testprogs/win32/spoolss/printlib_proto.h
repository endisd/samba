#ifndef __PRINTLIB_H__
#define __PRINTLIB_H__

#undef _PRINTF_ATTRIBUTE
#define _PRINTF_ATTRIBUTE(a1, a2) PRINTF_ATTRIBUTE(a1, a2)
/* This file was automatically generated by mkproto.pl. DO NOT EDIT */

/* this file contains prototypes for functions that are private
 * to this subsystem or library. These functions should not be
 * used outside this particular subsystem! */


/* The following definitions come from printlib.c  */

void print_devmode (DEVMODE *pDevModeIn);
void print_acl (const char* str, ACL *acl);
void print_sid (const char* str, SID *sid);
void print_secdesc (SECURITY_DESCRIPTOR *secdesc);
void PrintLastError();
void print_job_info_1(PJOB_INFO_1 info);
void print_job_info_2(PJOB_INFO_2 info);
void print_job_info_3(PJOB_INFO_3 info);
void print_monitor_info_1 (PMONITOR_INFO_1 info);
void print_monitor_info_2 (PMONITOR_INFO_2 info);
void print_form_info_1 (PFORM_INFO_1 info);
void print_printer_info_1 (PPRINTER_INFO_1 info);
void print_printer_info_2 (PPRINTER_INFO_2 info);
void print_printer_info_5 (PPRINTER_INFO_5 info);
void print_printer_info_6 (PPRINTER_INFO_6 info);
void print_printer_info_7 (PPRINTER_INFO_7 info);
void print_printer_info_8 (PPRINTER_INFO_8 info);
void print_printer_info_9 (PPRINTER_INFO_9 info);
void print_printer_info_bylevel(DWORD level, LPBYTE buffer, DWORD count);
void print_printprocessor_info_1 (PPRINTPROCESSOR_INFO_1 info);
void print_driver_info_1 (PDRIVER_INFO_1 info);
void print_driver_info_2 (PDRIVER_INFO_2 info);
void print_driver_info_3 (PDRIVER_INFO_3 info);
void print_driver_info_4 (PDRIVER_INFO_4 info);
void print_driver_info_5 (PDRIVER_INFO_5 info);
void print_driver_info_6 (PDRIVER_INFO_6 info);
void print_driver_info_8 (PDRIVER_INFO_8 info);
void print_driver_info_bylevel(DWORD level, LPBYTE buffer, DWORD count);
void print_doc_info_1 (PDOC_INFO_1 info);
void print_printer_enum_values (PRINTER_ENUM_VALUES *info);
void print_printer_keys(LPSTR buffer);
void print_printer_data(LPSTR keyname, LPSTR valuename, DWORD size, LPBYTE buffer, DWORD type);
#undef _PRINTF_ATTRIBUTE
#define _PRINTF_ATTRIBUTE(a1, a2)

#endif /* __PRINTLIB_H__ */

