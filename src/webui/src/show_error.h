/*
 * show_error.h
 *
 *  Created on: 14 nov 2016
 *      Author: nop
 */

#ifndef _SHOW_ERROR_H_
#define _SHOW_ERROR_H_
#include <kore.h>
#include <mysql.h>
#include "attributes_set.h"


void show_mysql_error(MYSQL *mysql,attributes_set_t *ptr_attrl);

#endif /* SRC_SHOW_ERROR_H_ */
