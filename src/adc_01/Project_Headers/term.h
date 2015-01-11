/**
 *--------------------------------------------------------------------\n
 *          HSLU T&A Hochschule Luzern Technik+Architektur            \n
 *--------------------------------------------------------------------\n
 *
 * \brief         terminal with command line parser
 * \file
 * \author        Christian Jost, christian.jost@hslu.ch
 * \date          30.11.2012
 *
 * \b Language:   Ansi-C \n\n
 * \b Target:     MC-Car  \n
 *
 * \par Description:
 *
 * $Id: term.h 547 2013-12-01 18:41:02Z zajost $
 *--------------------------------------------------------------------
 */
#ifndef TERM_H
#define TERM_H

void termWriteNum16s(int16 value);
void termWriteNum16u(uint16 value);

void termWriteSubString(char *str, uint8 start, uint8 length);
void termWriteLine(char *str);
void termWrite(char *str);
void termWriteChar(char ch);

void termDoWork(void);
void termInit(void);

#endif /* TERM_H */
