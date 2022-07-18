#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <string.h>
#include <malloc.h>
#include <errno.h>
#include <string.h>

#include "server.h"

int get_A_Record(unsigned char** addr, const char domain_name[])
{
  char records[] = {3, 'f', 'o', 'o', 3, 'b', 'a', 'r', 3, 'l', 'a', 'n', 0};
  if (strcmp(records, domain_name) == 0) {
    *addr = malloc(4 * sizeof(unsigned char));
    if (*addr == NULL) {
      return -1;
    }
    (*addr)[0] = 192;
    (*addr)[1] = 0;
    (*addr)[2] = 33;
    (*addr)[3] = 8;
    // The IP of www.iana.org
    return 0;
  } else {
    return -1;
  }
}

int get_CNAME_Record(unsigned char** cname, const char domain_name[])
{
  char records[] = {3, 'f', 'o', 'o', 3, 'b', 'a', 'r', 3, 'l', 'a', 'n', 0};
  if (strcmp(records, domain_name) == 0) {
    char tmp[] = {3, 'w', 'w', 'w', 7, 'e', 'x', 'a', 'm', 'p', 'l', 'e', 3, 'c', 'o', 'm', 0};
    size_t cname_len = sizeof(tmp) / sizeof(char);
    *cname = malloc(cname_len);
    if (*cname == NULL) {
      return -1;
    }
    memcpy(*cname, tmp, cname_len);
    return 0;
  } else {
    return -1;
  }
}

int get_TXT_Record(unsigned char** txt_data, const char domain_name[])
{
  char records[] = {3, 'f', 'o', 'o', 3, 'b', 'a', 'r', 3, 'c', 'o', 'm', 0};
  if (strcmp(records, domain_name) == 0) {
    *txt_data = "Hello world! This is a dummy TXT record";
    return 0;
  } else {
    return -1;
  }
}

void print_resource_record(struct ResourceRecord *rr)
{
  int i;
  while (rr) {
    printf("  ResourceRecord { name '%s', type %u, class %u, ttl %u, rd_length %u, ",
      rr->name,
      rr->type,
      rr->class,
      rr->ttl,
      rr->rd_length
   );

    unsigned char* rd = rr->rd_data;
    switch (rr->type) {
      case A_Resource_RecordType:
        printf("Address Resource Record { address ");

        for (i = 0; i < 4; ++i)
          printf("%s%u", (i ? "." : ""), rd[i]);

        printf(" }");
        break;
      case CNAME_Resource_RecordType:
        printf("CNAME Resource Record { cname_data '%s' }", rd);
        break;
      case TXT_Resource_RecordType:
        printf("Text Resource Record { txt_data '%s' }", rd);
        break;
      default:
        printf("Unknown Resource Record { type: %d }", rr->type);
    }
    printf("}\n");
    rr = rr->next;
  }
}

void print_message(struct Message *msg)
{
  struct Question *q;

  printf("QUERY {\n");
  printf("  ID:                                %02x,\n", msg->id);
  printf("  QR (Query/Response flag):          %u,\n", msg->qr);
  printf("  OpCode:                            %u,\n", msg->opcode);
  printf("  QDcount (Question Count):          %u,\n", msg->qdCount);
  printf("  ANcount (Answer Record Count):     %u,\n", msg->anCount);
  printf("  NScount (Authority Record Count):  %u,\n", msg->nsCount);
  printf("  ARcount (Additional Record Count): %u,\n", msg->arCount);

  printf("  Questions: [\n");
  q = msg->questions;
  while (q) {
    printf("    {qName: '%s', qType: %u, qClass: %u}",
      q->qName,
      q->qType,
      q->qClass
    );
    q = q->next;
    q == NULL ? printf("\n") : printf(",\n");
  }
  printf("  ]\n");

  print_resource_record(msg->answers);
  print_resource_record(msg->authorities);
  print_resource_record(msg->additionals);

  printf("}\n");
}


/*
* Basic memory operations.
*/

/**
 * @brief Get 2 bytes(16 bits) from the pointer and move the pointer forward by 2 bytes.
 * The function does NOT have any mechanism to prevent the pointer from moving beyond
 * its boundary.
 * @param buffer pointer of pointer pointing to memory blocks where 2 bytes will be extracted
 * @returns 2 bytes as uint16_t. The variable will be in host byte order.
 */
uint16_t get_and_move_by_2bytes(const uint8_t** buffer)
{
  uint16_t value;

  memcpy(&value, *buffer, 2);
  *buffer += 2;

  return ntohs(value);
}

void set_and_move_by_1byte(uint8_t **buffer, uint8_t value)
{
  memcpy(*buffer, &value, 1);
  *buffer += 1;
}

void set_and_move_by_2bytes(uint8_t **buffer, uint16_t value)
{
  value = htons(value);
  memcpy(*buffer, &value, 2);
  *buffer += 2;
}

void set_and_move_by_4bytes(uint8_t **buffer, uint32_t value) {
  value = htonl(value);
  memcpy(*buffer, &value, 4);
  *buffer += 4;
}

void encode_domain_name(uint8_t **buffer, const char *domain) {
  strcpy(*buffer, domain);
  *buffer += (strlen(domain) + 1);
  return;
}


void parse_dns_query_header(struct Message *msg, const unsigned char** buffer) {
  msg->id = get_and_move_by_2bytes(buffer);

  uint32_t fields = get_and_move_by_2bytes(buffer);
  /*
   * Endianness only matters for layout of data in memory. As soon as data is loaded by the processor to be operated on,
   * endianness is completely irrelevent. Shifts, bitwise operations, and so on perform as you would expect
   * (data logically laid out as low-order bit to high) regardless of endianness.
   */
  msg->qr     = (fields & 0b1000000000000000) >> 15;
  msg->opcode = (fields & 0b0111100000000000) >> 11;
  msg->aa     = (fields & 0b0000010000000000) >> 10;
  msg->tc     = (fields & 0b0000001000000000) >> 9;
  msg->rd     = (fields & 0b0000000100000000) >> 8;
  msg->ra     = (fields & 0b0000000010000000) >> 7;
  // three reserved bits are not used.
  msg->rcode  = (fields & 0b0000000000001111) >> 0;

  msg->qdCount = get_and_move_by_2bytes(buffer);
  msg->anCount = get_and_move_by_2bytes(buffer);
  msg->nsCount = get_and_move_by_2bytes(buffer);
  msg->arCount = get_and_move_by_2bytes(buffer);
}

void encode_header(struct Message *msg, uint8_t **buffer)
{
  set_and_move_by_2bytes(buffer, msg->id);

  int fields = 0;
  fields |= (msg->qr << 15) & 0b1000000000000000;
  fields |= (msg->rcode << 0) & RCODE_MASK;
  // TODO: insert the rest of the fields
  set_and_move_by_2bytes(buffer, fields);

  set_and_move_by_2bytes(buffer, msg->qdCount);
  set_and_move_by_2bytes(buffer, msg->anCount);
  set_and_move_by_2bytes(buffer, msg->nsCount);
  set_and_move_by_2bytes(buffer, msg->arCount);
}

/**
 * @brief Parse the incoming bytes as a DNS message and store the parsed message
 * to msg
 * @param msg pointer to the struct where the parsed DNS message will be stored.
 * @param buffer pointer to bytes read from socket.
 * @param buffer_len number of bytes in buffer
 * @returns 0 means okay
 */
int parse_dns_query(struct Message *msg, const unsigned char* buffer, int buffer_len)
{
  if (buffer_len < HEADER_SIZE) {
    fprintf(stderr, "Data length incorrect\n");
    return -1;
  }
  parse_dns_query_header(msg, &buffer);

  if (msg->anCount != 0 || msg->nsCount != 0) {
    fprintf(stderr, "The DNS message contains unsupported section\n");
    return -1;
  }

  // parse questions one by one
  for (int i = 0; i < msg->qdCount; ++i) {
    struct Question *q = malloc(sizeof(struct Question));
    

    if (strlen(buffer) > 64 * 4 + 4) {
      // This works because a valid query contains something like 
      // 3www6google3com0, so it is null-terminated by default. Note that in
      // this implementation we dont really protect against intentionally malformed
      // query.
      // But what if a malformed message intentionally excludes all '\0'? It seems
      // to cause undefined behaviors!
      printf("The qName section appears to be longer than it should: %s\n", buffer);
      return -1;
    }
    q->qName = calloc(strlen(buffer), sizeof(char));
    strcpy(q->qName, buffer);
    
    buffer += (strlen(q->qName) + 1);
    if (q->qName == NULL) {
      printf("Failed to decode domain name!\n");
      return -1;
    }
    q->qType = get_and_move_by_2bytes(&buffer);
    q->qClass = get_and_move_by_2bytes(&buffer);    
    if (q->qClass != 1 && q->qClass != 255) {
      printf("qClass is equal to %d, which is currently not supported\n", q->qClass);
      return -1;
    }

    // prepend question to questions list: msg->questions is a pointer pointing
    // to the first question in a linked list of q's.
    q->next = msg->questions;
    msg->questions = q;
  }

  // We do not expect any resource records to parse here.

  return 0;
}

/**
 * @brief For every question in the message add an appropiate resource record
 * in either section 'answers', 'authorities' or 'additionals'.
 * 
 * @param msg a DNS query message
 */
void resolve_query(struct Message *msg)
{
  struct ResourceRecord *beg;
  struct ResourceRecord *rr;
  struct Question *q;
  int rc = -1; // should stand for return code

  // leave most values intact for response
  msg->qr = 1; // this is a response
  msg->aa = 1; // this server is authoritative
  msg->ra = 0; // no recursion available
  msg->rcode = Ok_ResponseType;

  // should already be 0
  msg->anCount = 0;
  msg->nsCount = 0;
  msg->arCount = 0;

  // for every question append resource records
  q = msg->questions;
  while (q) {
    rr = malloc(sizeof(struct ResourceRecord));
    memset(rr, 0, sizeof(struct ResourceRecord));

    rr->name = strdup(q->qName);
    /*
      The strdup() function returns a pointer to a new string which is
      a duplicate of the string s.  Memory for the new string is
      obtained with malloc(3), and can be freed with free(3).
    */
    rr->type = q->qType;
    rr->class = q->qClass;
    rr->ttl = 60*60; // in seconds; 0 means no caching

    // We only can only answer three question types so far
    // and the answer (resource records) will be all put
    // into the answers list.
    // This behavior is probably non-standard!
    switch (q->qType) {
      case A_Resource_RecordType:
        rr->rd_length = 4;
        rc = get_A_Record(&rr->rd_data, q->qName);
        if (rc < 0) {
          free(rr->name);
          free(rr);
        }
        break;
      case CNAME_Resource_RecordType:        
        rc = get_CNAME_Record(&rr->rd_data, q->qName);        
        if (rc < 0) {
          free(rr->name);
          free(rr);
        }
        rr->rd_length = strlen(rr->rd_data) + 1;
        break;
      case TXT_Resource_RecordType:
        rc = get_TXT_Record(&rr->rd_data, q->qName);
        if (rc < 0) {
          free(rr->name);
          free(rr);
        }
        rr->rd_length = strlen(rr->rd_data) + 1;
        // extra one byte for length of txt string: https://en.wikipedia.org/wiki/TXT_record
        break;
      /*
      case NS_Resource_RecordType:
      case CNAME_Resource_RecordType:
      case SOA_Resource_RecordType:
      case PTR_Resource_RecordType:
      case MX_Resource_RecordType:
      case TXT_Resource_RecordType:
      */
      default:
        free(rr);
        msg->rcode = NotImplemented_ResponseType;
        printf("Cannot answer question of type %d.\n", q->qType);
    }
    if (rc == 0) {
      msg->anCount++;
      // prepend resource record to answers list
      beg = msg->answers;
      msg->answers = rr;
      rr->next = beg;
    }
    q = q->next;
  }
}

/**
 * @brief 
 * 
 * @param rr 
 * @param buffer 
 * @return int 0 means success; 1: failure
 */
int encode_resource_records(struct ResourceRecord *rr, uint8_t **buffer)
{
  int i;
  while (rr) {
    // Answer questions by attaching resource sections.
    encode_domain_name(buffer, rr->name);
    set_and_move_by_2bytes(buffer, rr->type);
    set_and_move_by_2bytes(buffer, rr->class);
    set_and_move_by_4bytes(buffer, rr->ttl);
    set_and_move_by_2bytes(buffer, rr->rd_length);

    switch (rr->type) {
      case A_Resource_RecordType:
        for (i = 0; i < rr->rd_length; ++i) {
          set_and_move_by_1byte(buffer, rr->rd_data[i]);
        }
        break;
      case CNAME_Resource_RecordType:
        for (i = 0; i < rr->rd_length; i++)
          set_and_move_by_1byte(buffer, rr->rd_data[i]);
        break;
      case TXT_Resource_RecordType:
        set_and_move_by_1byte(buffer, rr->rd_length - 1);
        // minus one byte, which is for txt string length
        for (i = 0; i < rr->rd_length; i++)
          set_and_move_by_1byte(buffer, rr->rd_data[i]);
        break;
      default:
        fprintf(stderr, "Unknown type %u. => Ignore resource record.\n", rr->type);
      return 1;
    }

    rr = rr->next;
  }

  return 0;
}

/**
 * @brief 
 * 
 * @param msg 
 * @param buffer 
 * @return int 0 means success, 1 means failure
 */
int encode_msg(struct Message *msg, uint8_t **buffer)
{
  struct Question *q;
  int rc;

  encode_header(msg, buffer);

  q = msg->questions;
  while (q) {
    encode_domain_name(buffer, q->qName);
    set_and_move_by_2bytes(buffer, q->qType);
    set_and_move_by_2bytes(buffer, q->qClass);

    q = q->next;
  }

  rc = 0;
  rc |= encode_resource_records(msg->answers, buffer);
  rc |= encode_resource_records(msg->authorities, buffer);
  rc |= encode_resource_records(msg->additionals, buffer);

  return rc;
}

void free_resource_records(struct ResourceRecord *rr)
{
  struct ResourceRecord *next;

  while (rr) {
    free(rr->name);
    if (rr->rd_data != NULL) {
      free(rr->rd_data);
    }
    next = rr->next;
    free(rr);    
    rr = next;
  }
}

void free_questions(struct Question *qq)
{
  struct Question *next;

  while (qq) {
    free(qq->qName);
    next = qq->next;
    free(qq);
    qq = next;
  }
}

int main() {
  //buffer used to store bytes read from UDP socket
  uint8_t read_buf[READ_BUF_SIZE];
  struct sockaddr_in client_addr;
  socklen_t addr_len = sizeof(struct sockaddr_in);
  struct sockaddr_in addr;
  int nbytes;
  int sock;

  struct Message msg;
  memset(&msg, 0, sizeof(struct Message));

  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(PORT);

  sock = socket(AF_INET, SOCK_DGRAM, 0);

  if (bind(sock, (struct sockaddr*) &addr, addr_len) != 0) {
		perror("In bind()");
		return 1;
  }

  printf("Listening on port %u.\n", PORT);

  while (1) {
    free_questions(msg.questions);
    free_resource_records(msg.answers);
    free_resource_records(msg.authorities);
    free_resource_records(msg.additionals);
    memset(&msg, 0, sizeof(struct Message));

    nbytes = recvfrom(sock, read_buf, sizeof(read_buf), 0, (struct sockaddr *) &client_addr, &addr_len);

    if (parse_dns_query(&msg, read_buf, nbytes) != 0) {
      continue;
    }
    printf("-----===== DNS request received =====-----\n");
    printf("          ===== Question =====\n");
    print_message(&msg);

    /* Resolve query and put the answers into the query message */
    resolve_query(&msg);

    printf("\n          ===== Response =====\n");
    print_message(&msg);
    printf("\n\n");

    uint8_t *p = read_buf;
    if (encode_msg(&msg, &p) != 0) {
      continue;
    }
    /* Send DNS response */
    int buflen = p - read_buf;
    sendto(sock, read_buf, buflen, 0, (struct sockaddr*) &client_addr, addr_len);
  }
}
