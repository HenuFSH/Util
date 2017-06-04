#include <stdio.h>
#include <stdbool.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

typedef void (*destroy_list)();
typedef void (*parse)(xmlNodePtr cur);
typedef void (*link)();
struct parser {
  struct parser* next_ptr;
  char* xpath_expr;
  destroy_list free_list;
  parse parse_node;
  link link_node;
};
struct parser* parser_list;

void create_parser(char* xpath_expr, parse parse_node, destroy_list free_list, link link_node)
{
    struct parser* parser_entry;
    static struct parser* parser_previous_loop_ptr;
    parser_entry = calloc(1, sizeof(struct parser));
    if ( parser_entry == NULL ) {
        printf("out of memory./n");
        return ;
    }
    if ( xpath_expr != NULL && free_list != NULL && parse_node != NULL ) {
        parser_entry->xpath_expr = xpath_expr;
        parser_entry->free_list = free_list;
        parser_entry->parse_node = parse_node;
        parser_entry->link_node = link_node;
    }
    else {
        printf("Create parse error.");
    }

    if ( parser_list == NULL ) {
        parser_list = parser_entry;
    }
    else {
        parser_previous_loop_ptr->next_ptr = parser_entry;
    }
    parser_previous_loop_ptr = parser_entry;
    return;
}

static char * copy_str (char * src)
{
    int copy_len;
    char * copy;

    if (src) {  // only if not null
      copy_len = strlen (src);
      copy = malloc (copy_len + 1);
      strncpy (copy, src, copy_len);
      copy[copy_len] = 0;
      return copy;
    }
    else {
      return NULL;
    }
}

static int copy_int (char * src)
{
    if (src) {
      return atoi (src);
    }
    else {
      return 0;
    }
}

static bool copy_bool (char * src)
{
    if ( strcmp( src, "true" ) == 0 ) {
        return true;
    } else {
        return false;
    }
}

struct volumeLogEntry {
   struct volumeLogEntry *next_ptr;
   int  volumeIndex;
   char volumeID[40];
   char *display_name;
   int  sync_percentage;
   char *storage_group;
   char *used_by;
   bool is_system;
   bool is_iso;
   char *standing_state;
   char *replication_status;
};
struct volumeLogEntry *volumeTableList = NULL;

struct vmLogEntry {
   struct vmLogEntry *next_ptr;
   int  vmIndex;
   char vmID[40];
   char *display_name;
   char *running_node;
   char *availability;
   char *standing_state;
   bool can_start;
   bool data_lost_access;
   char *displaystate;
   bool has_template;
};
struct vmLogEntry *vmTableList  = NULL;

struct storageGroupLogEntry {
   struct storageGroupLogEntry *next_ptr;
   char sgID[40];
   char *name;
};
struct storageGroupLogEntry *sgList = NULL;

struct hostLogEntry
{
  struct  hostLogEntry *next_ptr;
  char hostID[40];
  char* name;
};
struct hostLogEntry *hostList = NULL;

struct vmHostLinkEntry
{
  struct  vmHostLinkEntry *next_ptr;
  char consumer[40];
  char supplier[40];
};

struct vmHostLinkEntry *vm_hostlinkList = NULL;

struct volumeSGLinkEntry
{
  struct  volumeSGLinkEntry *next_ptr;
  char consumer[40];
  char supplier[40];
};
struct volumeSGLinkEntry *volume_sglinkList = NULL;

struct volumeVMLinkEntry
{
  struct  volumeVMLinkEntry *next_ptr;
  char a[40];
  char b[40];
};
struct volumeVMLinkEntry *volume_vmlinkList = NULL;

struct volumeHostLinkEntry
{
  struct  volumeHostLinkEntry *next_ptr;
  char consumer[40];
  char supplier[40];
};
struct volumeHostLinkEntry *volume_hostlinkList = NULL;

int vm_index = 0;
int volume_index = 0;

void parse_volume(xmlNodePtr cur)
{
    if( cur == NULL ) {
        printf("volume node is empty.");
        return;
    }
    char* temp_value;
    struct volumeLogEntry *volume_entry;
    static struct volumeLogEntry *volume_previous_loop_ptr;
    volume_entry = calloc( 1, sizeof(struct volumeLogEntry) );
    if( volume_entry == NULL ) {
        printf("out of memory./n");
        return ;
    }

    temp_value = xmlGetProp(cur, (const xmlChar *) "id") ;
    if ( temp_value != NULL ) {
        strcpy(volume_entry->volumeID, temp_value);
        free(temp_value);
        temp_value = NULL;
    }
    else {
        printf("The id attribute of volume is empty.");
        return;
    }

    cur = cur->xmlChildrenNode;
    while ( cur != NULL ) {
        if( xmlStrcmp(cur->name, (const xmlChar *) "name") == 0 ) {
            volume_entry->display_name = xmlNodeGetContent(cur);
        }

        if( xmlStrcmp(cur->name, (const xmlChar *) "sync-percentage") == 0 ) {
            temp_value = xmlNodeGetContent(cur);
            /*
             *  If we got a value, convert the string to an integer.
             */
            if ( temp_value != NULL ) {
                volume_entry->sync_percentage = copy_int(temp_value);
                free(temp_value);
                temp_value = NULL;
             }
        }

        if( xmlStrcmp(cur->name, (const xmlChar *) "is-system") == 0 ) {
            temp_value = xmlNodeGetContent(cur);
            /*
             *  If we got a value, convert the string to an boolean.
             */
            if ( temp_value != NULL ) {
                volume_entry->is_system = copy_bool(temp_value);
                free(temp_value);
                temp_value = NULL;
             }
        }

        if( xmlStrcmp(cur->name, (const xmlChar *) "isiso") == 0 ) {
            temp_value = xmlNodeGetContent(cur);
            /*
             *  If we got a value, convert the string to an boolean.
             */
            if ( temp_value != NULL ) {
                volume_entry->is_iso = copy_bool(temp_value);
                free(temp_value);
                temp_value = NULL;
             }
        }

        if( xmlStrcmp(cur->name, (const xmlChar *) "standing-state") == 0 ) {
            volume_entry->standing_state = xmlNodeGetContent(cur);
        }

        if( xmlStrcmp(cur->name, (const xmlChar *) "replication-status") == 0 ) {
            volume_entry->replication_status =xmlNodeGetContent(cur);
        }

        cur = cur->next;
    }

    if( volumeTableList == NULL ) {
        volume_index = 1;
        volumeTableList = volume_entry;
    }
    else {
        volume_index = volume_index + 1;
        volume_previous_loop_ptr->next_ptr = volume_entry;
    }
    volume_entry->volumeIndex = volume_index;
    volume_previous_loop_ptr = volume_entry;
}

void parse_vm(xmlNodePtr cur)
{
    if( cur == NULL ) {
        printf("Vm node is empty.");
        return;
    }
    char* temp_value;
    struct vmLogEntry *vm_entry;
    static struct vmLogEntry *vm_previous_loop_ptr;
    vm_entry = calloc( 1, sizeof(struct vmLogEntry) );
    if( vm_entry == NULL ) {
        printf("out of memory./n");
        return ;
    }
    temp_value = xmlGetProp(cur, (const xmlChar *) "id") ;
    if ( temp_value != NULL ) {
        strcpy(vm_entry->vmID, temp_value);
        free(temp_value);
        temp_value = NULL;
    }
    else {
        printf("The id attribute of VM is empty.");
        return;
    }

    cur = cur->xmlChildrenNode;
    while ( cur != NULL ) {
        if( xmlStrcmp(cur->name, (const xmlChar *) "name") == 0 ) {
            vm_entry->display_name = xmlNodeGetContent(cur);
        }

        if( xmlStrcmp(cur->name, (const xmlChar *) "availability") == 0 ) {
            vm_entry->availability = xmlNodeGetContent(cur);
        }

        if( xmlStrcmp(cur->name, (const xmlChar *) "standing-state") == 0 ) {
            vm_entry->standing_state = xmlNodeGetContent(cur);
        }

        if( xmlStrcmp(cur->name, (const xmlChar *) "can-start") == 0 ) {
            temp_value = xmlNodeGetContent(cur);
            /*
             *  If we got a value, convert the string to an boolean.
             */
            if ( temp_value != NULL ) {
                vm_entry->can_start = copy_bool(temp_value);
                free(temp_value);
                temp_value = NULL;
             }
        }

        if( xmlStrcmp(cur->name, (const xmlChar *) "data-lost-access") == 0 ) {
            temp_value = xmlNodeGetContent(cur);
            /*
             *  If we got a value, convert the string to an boolean.
             */
            if ( temp_value != NULL ) {
                vm_entry->data_lost_access = copy_bool(temp_value);
                free(temp_value);
                temp_value = NULL;
             }
        }

        if( xmlStrcmp(cur->name, (const xmlChar *) "displaystate") == 0 ) {
            vm_entry->displaystate = xmlNodeGetContent(cur);
        }

        cur = cur->next;
    }

    if( vmTableList == NULL ) {
        vm_index = 1;
        vmTableList = vm_entry;
    }
    else {
        vm_index = vm_index + 1;
        vm_previous_loop_ptr->next_ptr = vm_entry;
    }
    vm_entry->vmIndex = vm_index;
    vm_previous_loop_ptr = vm_entry;
}

void parse_sg(xmlNodePtr cur)
{
    if( cur == NULL ) {
        printf("Storage group node is empty.");
        return;
    }
    char* temp_value;
    struct storageGroupLogEntry *sg_entry;
    static struct storageGroupLogEntry *sg_previous_loop_ptr;
    sg_entry = calloc( 1, sizeof(struct storageGroupLogEntry) );
    if( sg_entry == NULL ) {
        printf("out of memory./n");
        return ;
    }

    temp_value = xmlGetProp(cur, (const xmlChar *) "id") ;
    if ( temp_value != NULL ) {
        strcpy(sg_entry->sgID, temp_value);
        free(temp_value);
        temp_value = NULL;
    }
    else {
        printf("The id attribute of sg is empty.");
        return;
    }

    cur = cur->xmlChildrenNode;
    while ( cur != NULL ) {
        if( xmlStrcmp(cur->name, (const xmlChar *) "name") == 0 ) {
            sg_entry->name = xmlNodeGetContent(cur);
        }
        cur = cur->next;
    }

    if( sgList == NULL ) {
        sgList = sg_entry;
    }
    else {
        sg_previous_loop_ptr->next_ptr = sg_entry;
    }
    sg_previous_loop_ptr = sg_entry;
}

void parse_host(xmlNodePtr cur)
{
    if( cur == NULL) {
        printf("Host node is empty.");
        return;
    }
    char* temp_value;
    struct hostLogEntry *host_entry;
    static struct hostLogEntry *host_previous_loop_ptr;
    host_entry = calloc( 1, sizeof(struct hostLogEntry) );
    if( host_entry == NULL ) {
        printf("out of memory./n");
        return ;
    }
    temp_value = xmlGetProp(cur, (const xmlChar *) "id") ;
    if ( temp_value != NULL ) {
        strcpy(host_entry->hostID, temp_value);
        free(temp_value);
        temp_value = NULL;
    }
    else {
        printf("The id attribute of host is empty.");
        return;
    }

    cur = cur->xmlChildrenNode;
    while ( cur != NULL ) {
        if( xmlStrcmp(cur->name, (const xmlChar *) "name") == 0 ) {
           host_entry->name = xmlNodeGetContent(cur);
        }
        cur = cur->next;
    }

    if( hostList == NULL ) {
        hostList = host_entry;
    }
    else {
        host_previous_loop_ptr->next_ptr = host_entry;
    }
    host_previous_loop_ptr = host_entry;
}

void parse_vm_host_link(xmlNodePtr cur)
{
    if( cur == NULL) {
        printf("VM host link node is empty.");
        return;
    }
    char* temp_value;
    struct vmHostLinkEntry *vm_host_link_entry;
    static struct vmHostLinkEntry *vm_host_link_previous_loop_ptr;
    vm_host_link_entry = calloc( 1, sizeof(struct vmHostLinkEntry) );
    if( vm_host_link_entry == NULL ) {
        printf("out of memory./n");
        return ;
    }

    temp_value = xmlGetProp(cur, (const xmlChar *) "consumer");
    if( temp_value != NULL ) {
        strcpy( vm_host_link_entry->consumer, temp_value);
        free(temp_value);
        temp_value = NULL;
    }
    else {
        printf("The consumer attribute of vm-host link node is empty.");
        return;
    }

    temp_value = xmlGetProp(cur, (const xmlChar *) "supplier");
    if( temp_value != NULL ) {
        strcpy( vm_host_link_entry->supplier, temp_value);
        free(temp_value);
        temp_value = NULL;
    }
    else {
        printf("The supplier attribute of vm-host link node is empty.");
        return;
    }

    if ( vm_hostlinkList == NULL ) {
        vm_hostlinkList = vm_host_link_entry;
    }
    else {
        vm_host_link_previous_loop_ptr->next_ptr = vm_host_link_entry;
    }
    vm_host_link_previous_loop_ptr = vm_host_link_entry;
}

void parse_volume_sg_link(xmlNodePtr cur)
{
    if( cur == NULL) {
        printf("Volume storage group link node is empty.");
        return;
    }
    char* temp_value;
    struct volumeSGLinkEntry *volume_sg_link_entry;
    static struct volumeSGLinkEntry *volume_sg_link_previous_loop_ptr;
    volume_sg_link_entry = calloc( 1, sizeof(struct volumeSGLinkEntry) );
    if( volume_sg_link_entry == NULL ) {
        printf("out of memory./n");
        return ;
    }

    temp_value = xmlGetProp(cur, (const xmlChar *) "consumer");
    if( temp_value != NULL ) {
        strcpy( volume_sg_link_entry->consumer, temp_value);
        free(temp_value);
        temp_value = NULL;
    }
    else {
        printf("The consumer attribute of volume-storagegroup link node is empty.");
        return;
    }

    temp_value = xmlGetProp(cur, (const xmlChar *) "supplier");
    if( temp_value != NULL ) {
        strcpy( volume_sg_link_entry->supplier, temp_value);
        free(temp_value);
        temp_value = NULL;
    }
    else {
        printf("The supplier attribute of volume-storagegroup link node is empty.");
        return;
    }

    if ( volume_sglinkList == NULL ) {
        volume_sglinkList = volume_sg_link_entry;
    }
    else {
        volume_sg_link_previous_loop_ptr->next_ptr = volume_sg_link_entry;
    }
    volume_sg_link_previous_loop_ptr = volume_sg_link_entry;
}

void parse_volume_vm_link(xmlNodePtr cur)
{
    if( cur == NULL) {
        printf("Volume VM link node is empty.");
        return;
    }
    char* temp_value;
    struct volumeVMLinkEntry *volume_vm_link_entry;
    static struct volumeVMLinkEntry *volume_vm_link_previous_loop_ptr;
    volume_vm_link_entry = calloc( 1, sizeof(struct volumeVMLinkEntry) );
    if( volume_vm_link_entry == NULL ) {
        printf("out of memory./n");
        return ;
    }

    temp_value = xmlGetProp(cur, (const xmlChar *) "a");
    if( temp_value != NULL ) {
        strcpy( volume_vm_link_entry->a, temp_value);
        free(temp_value);
        temp_value = NULL;
    }
    else {
        printf("The a attribute of volume-storagegroup link node is empty.");
        return;
    }

    temp_value = xmlGetProp(cur, (const xmlChar *) "b");
    if( temp_value != NULL ) {
        strcpy( volume_vm_link_entry->b, temp_value);
        free(temp_value);
        temp_value = NULL;
    }
    else {
        printf("The b attribute of volume-storagegroup link node is empty.");
        return;
    }

    if ( volume_vmlinkList == NULL ) {
        volume_vmlinkList = volume_vm_link_entry;
    }
    else {
        volume_vm_link_previous_loop_ptr->next_ptr = volume_vm_link_entry;
    }
    volume_vm_link_previous_loop_ptr = volume_vm_link_entry;
}

void parse_volume_host_link(xmlNodePtr cur)
{
    if( cur == NULL) {
        printf("Volume host link node is empty.");
        return;
    }
    char* temp_value;
    struct volumeHostLinkEntry *volume_host_link_entry;
    static struct volumeHostLinkEntry *volume_host_link_previous_loop_ptr;
    volume_host_link_entry = calloc( 1, sizeof(struct volumeHostLinkEntry) );
    if( volume_host_link_entry == NULL ) {
        printf("out of memory./n");
        return ;
    }

    temp_value = xmlGetProp(cur, (const xmlChar *) "consumer");
    if( temp_value != NULL ) {
        strcpy( volume_host_link_entry->consumer, temp_value);
        free(temp_value);
        temp_value = NULL;
    }
    else {
        printf("The consumer attribute of volume-host link node is empty.");
        return;
    }

    temp_value = xmlGetProp(cur, (const xmlChar *) "supplier");
    if( temp_value != NULL ) {
        strcpy( volume_host_link_entry->supplier, temp_value);
        free(temp_value);
        temp_value = NULL;
    }
    else {
        printf("The supplier attribute of volume-host link node is empty.");
        return;
    }

    if ( volume_hostlinkList == NULL ) {
        volume_hostlinkList = volume_host_link_entry;
    }
    else {
        volume_host_link_previous_loop_ptr->next_ptr = volume_host_link_entry;
    }
    volume_host_link_previous_loop_ptr = volume_host_link_entry;
}

clock_t lasttime;
#include <time.h>
int parse_xml_file( char *xml_ptr ) {
     lasttime = clock();
     xmlDocPtr doc;
     xmlNodePtr cur;
     xmlXPathContextPtr xpathCtx;
     xmlXPathObjectPtr xpathObj;
#ifdef LIBXML_SAX1_ENABLED
    /*
     * build an XML tree from a the file;
     */
    doc = xmlParseFile(xml_ptr);
    if (doc == NULL) return(NULL);
#else
    /*
     * the library has been compiled without some of the old interfaces
     */
    return(NULL);
#endif /* LIBXML_SAX1_ENABLED */
    struct parser* parser_entry = parser_list;
    while ( parser_entry != NULL ) {
        if( parser_entry->free_list == NULL)
        {
            continue;
        }
        parser_entry->free_list();
        parser_entry = parser_entry->next_ptr;
    }
#if 1
    /* Create xpath evaluation context */
    xpathCtx = xmlXPathNewContext(doc);
    if(xpathCtx == NULL) {
        fprintf(stderr,"Error: unable to create new XPath context\n");
        xmlFreeDoc(doc);
        return(-1);
    }
    parser_entry = parser_list;
    while ( parser_entry != NULL ) {
        const xmlChar* xpathExpr = BAD_CAST parser_entry->xpath_expr;
        xpathObj = xmlXPathEvalExpression(xpathExpr, xpathCtx);
        if(xpathObj == NULL) {
            fprintf(stderr,"Error: unable to evaluate xpath expression \"%s\"\n", xpathExpr);
            continue;
        }

        /* Print results */
        xmlNodeSetPtr nodes = xpathObj->nodesetval;

        int size = (nodes) ? nodes->nodeNr : 0;
        int i = 0;
        for(i = 0; i < size; ++i) {
            parser_entry->parse_node(nodes->nodeTab[i]);
        }
        parser_entry = parser_entry->next_ptr;
    }
    /* Cleanup */
    xmlXPathFreeObject(xpathObj);
    xmlXPathFreeContext(xpathCtx);
    xmlFreeDoc(doc);
#else
    /*
     * Check the document is of the right kind
     */

    cur = xmlDocGetRootElement(doc);
    if (cur == NULL) {
        fprintf(stderr,"empty document\n");
        xmlFreeDoc(doc);
        return(NULL);
    }
    /*
     * Now, walk the tree.
     */
    cur = cur->xmlChildrenNode;

    while ( cur && xmlIsBlankNode ( cur ) ) {
        cur = cur -> next;
    }
    if ( cur == 0 ) {
        xmlFreeDoc(doc);
        return ( NULL );
    }

    cur = cur->xmlChildrenNode;
    cur = cur->xmlChildrenNode;

    while (cur != NULL) {
        if ( xmlStrcmp(cur->name, (const xmlChar *) "volume") == 0) {
          parse_volume(cur);
        }
        if ( xmlStrcmp(cur->name, (const xmlChar *) "vm") == 0) {
          parse_vm(cur);
        }
        if ( xmlStrcmp(cur->name, (const xmlChar *) "storagegroup") == 0) {
          parse_sg(cur);
        }
        if ( xmlStrcmp(cur->name, (const xmlChar *) "host") == 0) {
          parse_host(cur);
        }
        if ( xmlStrcmp(cur->name, (const xmlChar *) "link") == 0) {
            char* class_val = xmlGetProp(cur, (const xmlChar *) "class");
            char* name_val = xmlGetProp(cur, (const xmlChar *) "name");
            if( (strcmp(class_val, "consumer-supplier") == 0)
                && (strcmp(name_val, "vm-host") == 0 )) {
                parse_vm_host_link(cur);
            } else if ( (strcmp(class_val, "consumer-supplier") == 0)
                        && (strcmp(name_val, "volume-storagegroup") == 0 )) {
                parse_volume_sg_link(cur);

            } else if ( (strcmp(class_val, "consumer-supplier") == 0)
                        && (strcmp(name_val, "volume-host") == 0 )) {
                parse_volume_host_link(cur);

            } else if ( (strcmp(class_val, "a-b") == 0)
                       && (strcmp(name_val, "Volume-LastBootedVM") == 0 )) {
                parse_volume_vm_link(cur);

            }
            free(class_val);
            class_val = NULL;
            free(name_val);
            name_val = NULL;
        }
         cur = cur->next;
    }
#endif
    parser_entry = parser_list;
    while ( parser_entry != NULL ) {
        if( parser_entry->link_node == NULL)
        {
            parser_entry = parser_entry->next_ptr;
            continue;
        }
        parser_entry->link_node();
        parser_entry = parser_entry->next_ptr;
    }
    printf("Execute time : %d\n", (clock() - lasttime)*1000/CLOCKS_PER_SEC);
}
void free_vm_list()
{
    printf("free-vm\n");

}
void link_vm()
{
   printf("link-vm\n");
}
void free_volume_list()
{
   printf("free-volume\n");
}
void link_volume()
{
    printf("link-volume\n");
}
int populate_watch_list( char *xml_ptr ) {

    create_parser("//vm[@id]", parse_vm, free_vm_list, link_vm);
    create_parser("//volume[@id]", parse_volume, free_volume_list, link_volume);
    create_parser("//host[@id]", parse_host, free_volume_list, link_volume);
    create_parser("//storagegroup[@id]", parse_sg, free_volume_list, link_volume);
    create_parser("//link[@class='consumer-supplier' and @name='vm-host' and @consumer and @supplier]",
                  parse_vm_host_link, free_volume_list, NULL);
    create_parser("//link[@class='consumer-supplier' and @name='volume-storagegroup' and @consumer and @supplier]",
                  parse_volume_sg_link, free_volume_list, NULL);
    create_parser("//link[@class='consumer-supplier' and @name='volume-host' and @consumer and @supplier]",
                  parse_volume_host_link, free_volume_list, NULL);
    create_parser("//link[@class='a-b' and @name='Volume-LastBootedVM' and @a and @b]",
                  parse_volume_vm_link, free_volume_list, NULL);

    parse_xml_file(xml_ptr);
}

int main(int argc, char *argv[])
{
    /* COMPAT: Do not genrate nodes for formatting spaces */
    LIBXML_TEST_VERSION
    xmlKeepBlanksDefault(0);

    populate_watch_list("./xml");
    struct volumeLogEntry* volume_entry = volumeTableList;
    while (volume_entry != NULL) {
        printf("Id: %s\n", volume_entry->volumeID);
        printf("Name: %s\n", volume_entry->display_name);
        printf("Is_iso: %d\n", volume_entry->is_iso);
        printf("Is_system: %d\n", volume_entry->is_system);
        printf("Sync_percentage: %d\n", volume_entry->sync_percentage);
        printf("Standing_state: %s\n", volume_entry->standing_state);
        printf("Replication_status: %s\n", volume_entry->replication_status);

        volume_entry = volume_entry->next_ptr;

    }
    printf("=====================================================================\n");
    struct vmLogEntry* vm_entry = vmTableList;
    while (vm_entry != NULL) {
        printf("Id: %s\n", vm_entry->vmID);
        printf("Index: %d\n", vm_entry->vmIndex);
        printf("Name: %s\n", vm_entry->display_name);
        printf("Availability: %s\n", vm_entry->availability);
        printf("Standing_state: %s\n", vm_entry->standing_state);
        printf("Can-start: %d\n", vm_entry->can_start);
        printf("Standing_state: %d\n", vm_entry->data_lost_access);
        printf("Displaystate: %s\n", vm_entry->displaystate);

        vm_entry = vm_entry->next_ptr;

    }
    printf("=====================================================================\n");
    struct hostLogEntry* host_entry = hostList;
    while (host_entry != NULL) {
        printf("Id: %s\n", host_entry->hostID);
        printf("Name: %s\n", host_entry->name);

        host_entry = host_entry->next_ptr;
    }
    printf("=====================================================================\n");
    struct storageGroupLogEntry* sg_entry = sgList;
    while (sg_entry != NULL) {
        printf("Id: %s\n", sg_entry->sgID);
        printf("Name: %s\n", sg_entry->name);

        sg_entry = sg_entry->next_ptr;

    }
    printf("=====================================================================\n");
    struct vmHostLinkEntry* vm_host_link_entry = vm_hostlinkList;
    while (vm_host_link_entry != NULL) {
        printf("Consumer: %s\n", vm_host_link_entry->consumer);
        printf("Supplier: %s\n", vm_host_link_entry->supplier);

        vm_host_link_entry = vm_host_link_entry->next_ptr;

    }
    printf("=====================================================================\n");
    struct volumeSGLinkEntry* volume_sg_entry = volume_sglinkList;
    while (volume_sg_entry != NULL) {
        printf("Consumer: %s\n", volume_sg_entry->consumer);
        printf("Supplier: %s\n", volume_sg_entry->supplier);

        volume_sg_entry = volume_sg_entry->next_ptr;

    }
    printf("=====================================================================\n");
    struct volumeHostLinkEntry* volume_host_link_entry = volume_hostlinkList;
    while (volume_host_link_entry != NULL) {
        printf("Consumer: %s\n", volume_host_link_entry->consumer);
        printf("Supplier: %s\n", volume_host_link_entry->supplier);

        volume_host_link_entry = volume_host_link_entry->next_ptr;

    }
    printf("=====================================================================\n");
    struct volumeVMLinkEntry* volume_vm_link_entry = volume_vmlinkList;
    while (volume_vm_link_entry != NULL) {
        printf("Id: %s\n", volume_vm_link_entry->a);
        printf("Name: %s\n", volume_vm_link_entry->b);

        volume_vm_link_entry = volume_vm_link_entry->next_ptr;
    }
    /* Clean up everything else before quitting. */
    xmlCleanupParser();
    return 0;
}
