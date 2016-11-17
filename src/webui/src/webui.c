/*  Dowse - embedded WebUI based on Kore.io
 *
 *  (c) Copyright 2016 Dyne.org foundation, Amsterdam
 *  Written by Denis Roio aka jaromil <jaromil@dyne.org>
 *
 * This source code is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Public License as published
 * by the Free Software Foundation; either version 3 of the License,
 * or (at your option) any later version.
 *
 * This source code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * Please refer to the GNU Public License for more details.
 *
 * You should have received a copy of the GNU Public License along with
 * this source code; if not, write to:
 * Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */
#define __WEBUI_MAIN_FILE__
#include <webui.h>

int thing_show_cb( attributes_set_t  *data, int argc, MYSQL_ROW argv, MYSQL_FIELD *azColName) {
#define SIZE (256)
    int i;

    attributes_set_t t;   
    t=attrinit();
   
    for(i=0; i<argc; i++){ // save all fields into the template
      if(!(argv[i])) continue;
      
      if(strcmp(azColName[i].name,"last")==0) {
	char *humandate;
	humandate=(char*)calloc(1,SIZE);
	
	kore_log(LOG_DEBUG,"last: %s",argv[i]);
	relative_time(argv[i],humandate);
	t=attrcat(t, "last", humandate);
      } else if(strcmp(azColName[i].name,"age")==0) {
	char *humandate;
	humandate=(char*)calloc(1,SIZE);
	
	kore_log(LOG_DEBUG,"age: %s",argv[i]);
	relative_time(argv[i],humandate);
	t=attrcat(t, "age",  humandate);
      } else {
	char *key,*value;
	key=(char*)calloc(1,SIZE);
	value=(char*)calloc(1,SIZE);
	
	snprintf(key,SIZE,"%s",azColName[i].name);
	snprintf(value,SIZE,"%s",argv[i]);
	kore_log(LOG_DEBUG,"%s: [%s]",key,value);
	t=attrcat(t, key,value);
      }
    }
    //--- In the hashmap data we add to the key "things" the hm element we created.
    (*data)=attr_add(*data,"things",t);
    WEBUI_DEBUG
    return 1;

}


/* TODO Choose if purge or not.
 *  What list this callback ?
 *  How is it made the template?
 *
 * */
int things_list_cb(attributes_set_t *data, int argc, MYSQL_ROW argv, MYSQL_FIELD *azColName)
{
    int i;
    char *laststr;
    char humandate[256];
    attributes_set_t *attr;
     char *button_group_start="<div class=\"btn-group\" role=\"group\" aria-label=\"actions\">";
     char *button_start="<div type=\"button\" class=\"btn btn-default\">";
     attributes_set_t t;
     t=attrinit();

    WEBUI_DEBUG

    for(i=0; i<argc; i++){ // save all fields into the hashmap
        attrcat(t , azColName[i].name,
                    (argv[i] ? argv[i] : "NULL"));
    }

    attr=data;
    attrcat((attributes_set_t)data,"list_of_things","<tr>");

    snprintf(line,ml,
"<td><a href=\"/things?macaddr=%s\">"
"%s</td><td>%s</td></a>",
             thing_get("macaddr"),
             thing_get("hostname"), thing_get("os"));
    attrcat((attributes_set_t)data,"list_of_things",line);

    // get last datestamp
    laststr = thing_get("last");
    if(laststr) relative_time(laststr,humandate);
    snprintf(line, ml, "<td>%s</td><td>", humandate);
    attrcat((attributes_set_t)data,"list_of_things",line);

    // action buttons
    attrcat((attributes_set_t)data,"list_of_things",button_group_start);

    // info button
    attrcat((attributes_set_t)data,"list_of_things",button_start);
    snprintf(line,ml,
             "<a href=\"/things?macaddr=%s\">info</a></div>",
             thing_get("macaddr"));
    attrcat((attributes_set_t)data,"list_of_things",line);

    attrcat((attributes_set_t)data,"list_of_things","</div></td></tr>");

    // snprintf(line,ml,"<td>%s</td><td>%s</td>\n",
    //          thing_get("macaddr"), thing_get("ip4"));

    // attrcat(data,"list_of_things",line);


    return 0;
}

int thing_show(struct http_request *req) {
  u_int8_t	 *response;
  size_t len;  
  attributes_set_t attributes;
 
  WEBUI_DEBUG;
  http_populate_get(req);

  // we shouldn't free the result in macaddr
  if (http_argument_get_string(req, "macaddr", &macaddr)) {
    kore_log(LOG_DEBUG, "thing_show macaddr %s",macaddr);
    //--- prepare where condition
    snprintf(where_condition,ml,"WHERE macaddr='%s'",macaddr);
  } else {
    kore_log(LOG_ERR,"thing_show get argument error");
    kore_log(LOG_DEBUG,"thing_show called without argument");
    //--- prepare where condition
    snprintf(where_condition,ml," ");
  }
  
  WEBUI_DEBUG;
  // prepare query
  snprintf(line,ml,"SELECT * FROM found %s ORDER BY age DESC",where_condition);
  
  WEBUI_DEBUG;
  buf = kore_buf_alloc(mb);
  
  WEBUI_DEBUG;
  // load template
  template_load
    (asset_thing_show_html, asset_len_thing_show_html, &tmpl);
  WEBUI_DEBUG;
  attributes = attrinit();

  WEBUI_DEBUG;
  attributes=attrcat(attributes, "title", "Dowse information panel");
  
  WEBUI_DEBUG;
  // SQL query
  sqlquery(line, thing_show_cb, &attributes);
  
  template_apply(&tmpl,attributes,buf);
  
  WEBUI_DEBUG;
  response=kore_buf_release(buf,&len);
  
  http_response(req, 200, response,len);
  WEBUI_DEBUG;
  
  template_free(&tmpl);
  
  WEBUI_DEBUG;
  attrfree(attributes);
    
  //    kore_buf_free(buf);
  
  //  kore_free(data);
    
  return (KORE_RESULT_OK);
}

int things_list(struct http_request *req) {
    char *query = "SELECT * FROM found ORDER BY last DESC";
    template_t tmpl;
    attributes_set_t attributes;
    map_t thing = NULL;

    struct timespec when;
    WEBUI_DEBUG;
    buf = kore_buf_alloc(mb);

    thing = hashmap_new();

    // load template from assets
    template_load
        (asset_things_list_html, asset_len_things_list_html, &tmpl);

    // initialise attribute list
    attributes = attrinit();

    if( ! parse_datetime(&when, "now", NULL) )
        kore_log(LOG_ERR,"parse-datetime error");
    else {
        struct tm *tt;
        tt = localtime (&when.tv_sec);
        mktime(tt);
        strftime(line, ml, "Dowse :: %d %m %Y - %H:%M:%S", tt);
        attrcat(attributes, "title", line);
    }
    
    sqlquery(query, things_list_cb, &attributes);

    template_apply(&tmpl, attributes, buf);

	http_response(req, 200, buf->data, buf->offset);

    template_free(&tmpl);
    attrfree(attributes);

    kore_buf_free(buf);


	return (KORE_RESULT_OK);
}

char *thing_get(char *key) {
    int res;
    char *sval = NULL;
    /* TODO Delete ! It's no more needed by ctemplate integration */
    /*    res = hashmap_get(thing, key, (void**)&sval);
        if(res==MAP_OK)
            return(sval);
        else
            return("NULL");*/
    return "NULL";
}

