/********************************************************************\
 * gnc-ui-util.h -- utility functions for the GnuCash UI            *
 * Copyright (C) 2000 Dave Peticolas <dave@krondo.com>              *
 *                                                                  *
 * This program is free software; you can redistribute it and/or    *
 * modify it under the terms of the GNU General Public License as   *
 * published by the Free Software Foundation; either version 2 of   *
 * the License, or (at your option) any later version.              *
 *                                                                  *
 * This program is distributed in the hope that it will be useful,  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of   *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the    *
 * GNU General Public License for more details.                     *
 *                                                                  *
 * You should have received a copy of the GNU General Public License*
 * along with this program; if not, contact:                        *
 *                                                                  *
 * Free Software Foundation           Voice:  +1-617-542-5942       *
 * 59 Temple Place - Suite 330        Fax:    +1-617-542-2652       *
 * Boston, MA  02111-1307,  USA       gnu@gnu.org                   *
\********************************************************************/

/** @addtogroup GUI
    @{ */
/** @addtogroup GuiUtility Utility functions for the GnuCash GUI
 * @{ */
/** @file gnc-ui-util.h 
    @brief  utility functions for the GnuCash UI
    @author Copyright (C) 2000 Dave Peticolas <dave@krondo.com>
*/

#ifndef GNC_UI_UTIL_H
#define GNC_UI_UTIL_H

#include "config.h"

#include <glib.h>
#include <locale.h>

#include "Account.h"
#include "gnc-engine.h"
#include "Group.h"
#include "qofbook.h"
#include "qofsession.h"


typedef QofSession * (*QofSessionCB) (void);


/** Returns the account separation character chosen by the user.
 *
 *  @return The character to use.
 */
char gnc_get_account_separator (void);

gboolean gnc_reverse_balance(Account *account);
gboolean gnc_reverse_balance_type(GNCAccountType type);


/* Default directories **********************************************/

void gnc_init_default_directory (char **dirname);
/** 
 * Extracts the directory part of the given 'filename' into the char
 * pointer variable '*dirname'. If the 'filename' is NULL or does not
 * contain any directory separator '/', then '*dirname' will be set to
 * NULL.
 * 
 * WATCH OUT: If '*dirname' (i.e. the underlying char pointer
 * variable) is non-NULL, then it will be free()'d. Make sure that you
 * have initialized it to NULL, or otherwise you will get a bogus
 * free() or a double-free() here. FIXME: This is probably not-so-good
 * behaviour and should be changed (2005-10-08, cstim).
 *
 * Again watch out: The caller takes ownership of the char buffer
 * '*dirname', i.e. the caller has to do a g_free(*dirname) when that
 * buffer is no longer in use.
*/
void gnc_extract_directory (char **dirname, const char *filename);


/* Engine enhancements & i18n ***************************************/
QofBook * gnc_get_current_book (void);
AccountGroup * gnc_get_current_group (void);
gnc_commodity_table * gnc_get_current_commodities (void);

/*
 * This is a wrapper routine around an xaccGetBalanceInCurrency
 * function that handles additional needs of the gui.
 *
 * @param fn        The underlying function in Account.c to call to retrieve
 *                  a specific balance from the account.
 * @param account   The account to retrieve data about.
 * @param recurse   Include all sub-accounts of this account.
 * @param negative  An indication of whether or not the returned value
 *                  is negative.  This can be used by the caller to
 *                  easily decode whether or not to color the output.
 * @param commodity The commodity in which the account balance should
 *                  be returned. If NULL, the value will be returned in
 *                  the commodity of the account. This is normally used
 *                  to specify a currency, which forces the conversion
 *                  of things like stock account values from share
 *                  values to an amount the requested currency.
 */
gnc_numeric
gnc_ui_account_get_balance_full (xaccGetBalanceInCurrencyFn fn,
				 Account *account,
				 gboolean recurse,
				 gboolean *negative,
				 gnc_commodity *commodity);

/**
 * This routine retrives the total balance in an account, possibly
 * including all sub-accounts under the specified account.
 *
 * @param account           The account to retrieve data about.
 * @param include_children  Include all sub-accounts of this account.
 */
gnc_numeric gnc_ui_account_get_balance (Account *account,
                                        gboolean include_children);

gnc_numeric gnc_ui_account_get_balance_in_currency (Account *account,
						    gnc_commodity *currency,
						    gboolean recurse);
/**
 * This routine retrives the reconciled balance in an account,
 * possibly including all sub-accounts under the specified account.
 *
 * @param account           The account to retrieve data about.
 * @param include_children  Include all sub-accounts of this account.
 */
gnc_numeric gnc_ui_account_get_reconciled_balance(Account *account,
                                                  gboolean include_children);

/**
 * Wrapper around gnc_ui_account_get_balance_internal that converts
 * the resulting number to a character string.  The number is
 * formatted according to the specification of the account currency.
 *
 * @param fn        The underlying function in Account.c to call to retrieve
 *                  a specific balance from the account.
 * @param account   The account to retrieve data about.
 * @param recurse   Include all sub-accounts of this account.
 * @param negative  An indication of whether or not the returned value
 *                  is negative.  This can be used by the caller to
 *                  easily decode whether or not to color the output.
 */
gchar *
gnc_ui_account_get_print_balance (xaccGetBalanceInCurrencyFn fn,
				  Account *account,
				  gboolean recurse,
				  gboolean *negative);

/**
 * Wrapper around gnc_ui_account_get_balance_internal that converts
 * the resulting number to a character string.  The number is
 * formatted according to the specification of the default reporting
 * currency.
 *
 * @param fn        The underlying function in Account.c to call to retrieve
 *                  a specific balance from the account.
 * @param account   The account to retrieve data about.
 * @param recurse   Include all sub-accounts of this account.
 * @param negative  An indication of whether or not the returned value
 *                  is negative.  This can be used by the caller to
 *                  easily decode whether or not to color the output.
 */
gchar *
gnc_ui_account_get_print_report_balance (xaccGetBalanceInCurrencyFn fn,
					 Account *account,
					 gboolean recurse,
					 gboolean *negative);

char *gnc_ui_account_get_tax_info_string (Account *account);

gnc_numeric gnc_ui_account_get_balance_as_of_date (Account *account,
                                                   time_t date,
                                                   gboolean include_children);

const char * gnc_get_reconcile_str (char reconciled_flag);
const char * gnc_get_reconcile_valid_flags (void);
const char * gnc_get_reconcile_flag_order (void);

typedef enum
{
  EQUITY_OPENING_BALANCE,
  EQUITY_RETAINED_EARNINGS,
  NUM_EQUITY_TYPES
} GNCEquityType;

Account * gnc_find_or_create_equity_account (AccountGroup *group,
                                             GNCEquityType equity_type,
                                             gnc_commodity *currency,
                                             QofBook *book);
gboolean gnc_account_create_opening_balance (Account *account,
                                             gnc_numeric balance,
                                             time_t date,
                                             QofBook *book);

char * gnc_account_get_full_name (Account *account);


/* Locale functions *************************************************/

/* The gnc_localeconv() subroutine returns an lconv structure
 * containing locale information. If no locale is set, the structure
 * is given default (en_US) values.  */
struct lconv * gnc_localeconv (void);

/* Returns the default currency of the current locale, or NULL if no
 * sensible currency could be identified from the locale. */
gnc_commodity * gnc_locale_default_currency_nodefault (void);

/* Returns the default currency of the current locale. WATCH OUT: If
 * no currency could be identified from the locale, this one returns
 * "USD", but this will have nothing to do with the actual locale. */
gnc_commodity * gnc_locale_default_currency (void);

/* Returns the default ISO currency string of the current locale. */
const char * gnc_locale_default_iso_currency_code (void);


/** Return the default currency set by the user.  If the user's
 *  preference is invalid, then this routine will return the default
 *  currency for the user's locale.
 *
 *  @return A pointer to a currency.
 */
gnc_commodity * gnc_default_currency (void);


/** Return the default currency for use in reports, as set by the
 *  user.  If the user's preference is invalid, then this routine will
 *  return the default currency for the user's locale.
 *
 *  @return A pointer to a currency.
 */
gnc_commodity * gnc_default_report_currency (void);


/* Returns the number of decimal place to print in the current locale */
int gnc_locale_decimal_places (void);

/* Push and pop locales. Currently, this has no effect on gnc_localeconv.
 * i.e., after the first call to gnc_localeconv, subsequent calls will
 * return the same information. */
void gnc_push_locale (const char *locale);
void gnc_pop_locale (void);

/* Amount printing and parsing **************************************/

/*
 * The xaccPrintAmount() and xaccSPrintAmount() routines provide
 *    i18n'ed convenience routines for printing gnc_numerics.
 *    amounts. Both routines take a gnc_numeric argument and
 *    a printing information object.
 *
 * The xaccPrintAmount() routine returns a pointer to a statically
 *    allocated buffer, and is therefore not thread-safe.
 *
 * The xaccSPrintAmount() routine accepts a pointer to the buffer to be
 *    printed to.  It returns the length of the printed string.
 */

typedef struct _GNCPrintAmountInfo
{
  const gnc_commodity *commodity;  /* may be NULL */

  guint8 max_decimal_places;
  guint8 min_decimal_places;

  unsigned int use_separators : 1; /* Print thousands separators */
  unsigned int use_symbol : 1;     /* Print currency symbol */
  unsigned int use_locale : 1;     /* Use locale for some positioning */
  unsigned int monetary : 1;       /* Is a monetary quantity */
  unsigned int force_fit : 1;      /* Don't print more than max_dp places */
  unsigned int round : 1;          /* Round at max_dp instead of truncating */
} GNCPrintAmountInfo;


GNCPrintAmountInfo gnc_default_print_info (gboolean use_symbol);

GNCPrintAmountInfo gnc_commodity_print_info (const gnc_commodity *commodity,
                                             gboolean use_symbol);

GNCPrintAmountInfo gnc_account_print_info (Account *account,
                                           gboolean use_symbol);

GNCPrintAmountInfo gnc_split_amount_print_info (Split *split,
                                                gboolean use_symbol);
GNCPrintAmountInfo gnc_split_value_print_info (Split *split,
                                               gboolean use_symbol);

GNCPrintAmountInfo gnc_share_print_info_places (int decplaces);
GNCPrintAmountInfo gnc_default_share_print_info (void);
GNCPrintAmountInfo gnc_default_price_print_info (void);

GNCPrintAmountInfo gnc_integral_print_info (void);

const char * xaccPrintAmount (gnc_numeric val, GNCPrintAmountInfo info);
int xaccSPrintAmount (char *buf, gnc_numeric val, GNCPrintAmountInfo info);


/* xaccParseAmount parses in_str to obtain a numeric result. The
 *   routine will parse as much of in_str as it can to obtain a single
 *   number. The number is parsed using the current locale information
 *   and the 'monetary' flag. The routine will return TRUE if it
 *   successfully parsed a number and FALSE otherwise. If TRUE is
 *   returned and result is non-NULL, the value of the parsed number
 *   is stored in *result. If FALSE is returned, *result is
 *   unchanged. If TRUE is returned and endstr is non-NULL, the
 *   location of the first character in in_str not used by the parser
 *   will be returned in *endstr. If FALSE is returned and endstr is
 *   non-NULL, *endstr will point to in_str. */
gboolean xaccParseAmount (const char * in_str, gboolean monetary,
                          gnc_numeric *result, char **endstr);

/*
 * xaccParseAmountExtended is just like xaccParseAmount except the
 * caller must provide all the locale-specific information.
 *
 * Note: if group is NULL, no group-size verification will take place.
 * ignore_list is a list of characters that are completely ignored
 * while processing the input string.  If ignore_list is NULL, nothing
 * is ignored.
 */
gboolean
xaccParseAmountExtended (const char * in_str, gboolean monetary,
			 char negative_sign, char decimal_point,
			 char group_separator, char *group, char *ignore_list,
			 gnc_numeric *result, char **endstr);

/* Initialization ***************************************************/

void gnc_ui_util_init (void);

/* Missing functions ************************************************/

#ifndef HAVE_TOWUPPER
gint32 towupper (gint32 wc);
int iswlower (gint32 wc);
#endif

#endif
/** @} */
/** @} */
