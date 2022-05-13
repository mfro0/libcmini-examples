/* COLOR.RSH
 *==========================================================================
 * DATE: MAY 3, 1990	(from C. Gee)
 * DESCRIPTION: RSH file from RESOURCE
 */
#ifndef COLOR_RSH
#define COLOR_RSH

#if !defined(USA) && !defined(UK) && !defined(FRANCE) && !defined(GERMANY) && !defined(ITALY) && !defined(SPAIN) && !defined(SWEDEN)
#define USA
#endif

#if defined(USA)
#include "resource/usa/usa.rsh"
#include "resource/usa/USA.RH"
#endif

#if	UK
#include "resource/uk/uk.rsh"
#endif

#if FRANCE
#include "resource/france/france.rsh"
#endif

#if GERMANY
#include "resource/germany/germany.rsh"
#endif

#if ITALY
#include "resource/italy/italy.rsh"
#endif

#if SPAIN
#include "resource/spain/spain.rsh"
#endif

#if SWEDEN
#include "resource/sweden/sweden.rsh"
#endif

#endif  /* COLOR_RSH */

