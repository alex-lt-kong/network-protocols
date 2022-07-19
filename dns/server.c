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
    char tmp[] = {3, 'w', 'w', 'w', 4, 'i', 'a', 'n', 'a', 3, 'o', 'r', 'g', 0};
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
  char records[] = {3, 'f', 'o', 'o', 3, 'b', 'a', 'r', 3, 'l', 'a', 'n', 0};
  if (strcmp(records, domain_name) == 0) {
    char tmp[] = "Hello world! This is a dummy TXT record";
    //*txt_data = "Hello world! This is a dummy TXT record";
    size_t txt_data_len = sizeof(tmp) / sizeof(char);
    *txt_data = malloc(txt_data_len);
    if (*txt_data == NULL) {
      return -1;
    }
    memcpy(*txt_data, tmp, txt_data_len);
    return 0;
  } else {
    printf("OUT!\n");
    return -1;
  }
}

/**
 * @brief free() a ResourceRecord's members and itself and set. If ResourceRecord pointer
 * is NULL already, its members won't be free()'ed
 * 
 * 
 * @param rr 
 */
void free_resource_records(struct ResourceRecord **rr)
{
  if (rr != NULL) {
    free((*rr)->name);
    free((*rr)->rdata);
  }
  free(*rr);
  *rr = NULL;
}

void print_resource_record(struct ResourceRecord *rr)
{
  int i;
  printf("  ResourceRecord: {\n");
  printf("    name '%s';\n", rr->name);
  printf("    type %u;\n", rr->type);
  printf("    class %u;\n", rr->class);
  printf("    ttl %u;\n", rr->ttl);
  printf("    rd_length %u;\n", rr->rd_length);
  printf("    rdata: {\n");

  unsigned char* rd = rr->rdata;
  switch (rr->type) {
    case A_Resource_RecordType:
      printf("      IPv4address: ");
      for (i = 0; i < 4; ++i) {
        printf("%s%u", (i ? "." : ""), rd[i]); 
      }
      printf("\n");
      break;
    case CNAME_Resource_RecordType:
      printf("      CNAME: '%s'\n", rd);
      break;
    case TXT_Resource_RecordType:
      printf("      TXT: '%s'\n", rd);
      break;
    default:
      printf("<Unknown Resource Record>\n", rr->type);
  }  
  printf("    };\n");
  printf("  }\n");

}

void print_message(struct Message *msg)
{
  printf("QUERY {\n");
  printf("  ID:                                %02x;\n", msg->header->id);
  printf("  QR (Query/Response flag):          %u;\n", msg->header->qr);
  printf("  OpCode:                            %u;\n", msg->header->opcode);
  printf("  RCode (Response Code):             %u;\n", msg->header->rcode);
  printf("  QDcount (Question Count):          %u;\n", msg->header->qd_count);
  printf("  ANcount (Answer Record Count):     %u;\n", msg->header->an_count);
  printf("  NScount (Authority Record Count):  %u;\n", msg->header->ns_count);
  printf("  ARcount (Additional Record Count): %u;\n", msg->header->ar_count);

  printf("  Question: {\n");
  printf("    q_name:  '%s';\n", msg->question->q_name);
  printf("    q_type:  %u;\n", msg->question->q_type);
  printf("    q_class: %u;\n", msg->question->q_class);
  printf("  };\n");
  if (msg->header->qr == 1) {
    if (msg->header->rcode == NotImplemented_ResponseCode) {
      printf("  ResourceRecord: NULL (Question's RecordType %u has not been implemented);\n", msg->question->q_type);
    } else if (msg->header->an_count == 1) {
      print_resource_record(msg->answer); 
    } else {
      printf("  ResourceRecord: NULL (record not found);\n");
    }
  }
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


/**
 * @brief Parse the first 12 bytes of the buffer into DNS query message header.
 * There is no way for this function to know the exact size of the given buffer,
 * callers are required to make sure the buffer is at least 12-byte long to 
 * avoid OutOfBounds access
 * 
 * @param header The pointer to DNS query's header section 
 * @param buffer The pointer of pointer to the minimumn 12-byte buffer. Since we
 * need to move the pointer to the buffer, we have to pass a pointer to pointer
 * as a parameter here.
 */
void parse_dns_query_header(struct Header *header, const unsigned char** buffer) {
  header->id = get_and_move_by_2bytes(buffer);

  uint32_t fields = get_and_move_by_2bytes(buffer);
  /*
   * Endianness only matters for layout of data in memory. As soon as data is loaded by the processor to be operated on,
   * endianness is completely irrelevent. Shifts, bitwise operations, and so on perform as you would expect
   * (data logically laid out as low-order bit to high) regardless of endianness.
   */
  header->qr     = (fields & 0b1000000000000000) >> 15;
  header->opcode = (fields & 0b0111100000000000) >> 11;
  header->aa     = (fields & 0b0000010000000000) >> 10;
  header->tc     = (fields & 0b0000001000000000) >> 9;
  header->rd     = (fields & 0b0000000100000000) >> 8;
  header->ra     = (fields & 0b0000000010000000) >> 7;
  // three reserved bits are not used.
  header->rcode  = (fields & 0b0000000000001111) >> 0;

  header->qd_count = get_and_move_by_2bytes(buffer);
  header->an_count = get_and_move_by_2bytes(buffer);
  header->ns_count = get_and_move_by_2bytes(buffer);
  header->ar_count = get_and_move_by_2bytes(buffer);
}

void encode_header(struct Header *header, uint8_t **buffer)
{
  set_and_move_by_2bytes(buffer, header->id);

  int fields = 0;
  fields |= (header->qr << 15)   & 0b1000000000000000;
  fields |= (header->rcode << 0) & 0b0000000000001111;
  // TODO: insert the rest of the fields
  set_and_move_by_2bytes(buffer, fields);

  set_and_move_by_2bytes(buffer, header->qd_count);
  set_and_move_by_2bytes(buffer, header->an_count);
  set_and_move_by_2bytes(buffer, header->ns_count);
  set_and_move_by_2bytes(buffer, header->ar_count);
}

/**
 * @brief Parse the incoming bytes as a DNS message and store the parsed message
 * to msg
 * @param msg pointer to the struct where the parsed DNS message will be stored.
 * @param buffer pointer to bytes read from socket.
 * @param buffer_len number of bytes in buffer
 * @returns Follows the Response Code: NoError: 0, FormatError: 1, ServerFailure: 2
 */
int parse_dns_query(struct Message *msg, const unsigned char* buffer, int buffer_len)
{
  if (buffer_len < HEADER_SIZE) {
    fprintf(stderr, "Data length incorrect\n");
    return FormatError_ResponseCode;
  }
  msg->header = malloc(sizeof(struct Header));
  if (msg->header == NULL) {
    fprintf(stderr, "Failed to malloc() enough memory\n");
    return ServerFailure_ResponseCode;
  }
  parse_dns_query_header(msg->header, &buffer);

  if (
      msg->header->qr != 0 ||
      msg->header->an_count != 0 || msg->header->ns_count != 0 || msg->header->ar_count != 0
    ) {
    fprintf(stderr, "The DNS message contains invalid values\n");
    return FormatError_ResponseCode;
  }

  if (msg->header->qd_count != 1) {
    fprintf(stderr, "qd_count is not equal to 1, which is not supported by this program\n");
    return ServerFailure_ResponseCode;
  }

  msg->question = malloc(sizeof(struct Question));
  if (msg->question == NULL) {
    fprintf(stderr, "Failed to malloc() enough memory\n");
     return ServerFailure_ResponseCode;
  }

  short q_name_size = strnlen(buffer, READ_BUF_SIZE) + 1;
  // strnlen() works because a valid encoded domain name is something like 
  // 3www6google3com0, which is naturally null-terminated.
  // Note that we use strnlen(), a POSIX standard (i.e., not C standard)
  // function, instead of strlen(), to prevent reading beyond the array bounds.
  if (q_name_size > 64 * 4) {      
    fprintf(stderr, "The q_name section [%s] appears to be longer than it should\n", buffer);
    return -1;
  }
  msg->question->q_name = calloc(q_name_size, sizeof(char));
  if (msg->question->q_name == NULL) {
    fprintf(stderr, "Failed to calloc() memory to qName!\n");
    return FormatError_ResponseCode;
  }
  strcpy(msg->question->q_name, buffer);
  // calloc() + strcpy() keeps the last '\0', which is exactly what we need
  
  buffer += q_name_size;
  
  msg->question->q_type = get_and_move_by_2bytes(&buffer);
  msg->question->q_class = get_and_move_by_2bytes(&buffer);    
  if (msg->question->q_class != 1 && msg->question->q_class != 255) {
    printf("q_class is equal to %d, which is currently not supported\n", msg->question->q_class);
    return ServerFailure_ResponseCode;
  }

  return NoError_ResponseCode;
}

/**
 * @brief For every question in the message add an appropiate resource record
 * in either section 'answers', 'authorities' or 'additionals'.
 * 
 * @param msg a DNS query message
 */
int resolve_query(struct Message *msg)
{
  struct Question *q;
  int rc = -1; // should stand for return code

  // leave most values intact for response
  msg->header->qr = 1; // this is a response
  msg->header->aa = 1; // this server is authoritative
  msg->header->ra = 0; // no recursion available
  msg->header->rcode = NoError_ResponseCode;

  msg->header->an_count = 1;
  msg->header->ns_count = 0;
  msg->header->ar_count = 0;

  q = msg->question;


  msg->answer = calloc(1, sizeof(struct ResourceRecord));
  // Per DNS protocol, we can have three kinds of responses, answer/authority/additional.
  // This project implements only answer only.

  msg->answer->name = strdup(q->q_name);
  /*
    The strdup() function returns a pointer to a new string which is
    a duplicate of the string s.  Memory for the new string is
    obtained with malloc(3), and can be freed with free(3).
  */
  msg->answer->type = q->q_type;
  msg->answer->class = q->q_class;
  msg->answer->ttl = 60*60; // in seconds; 0 means no caching

  switch (q->q_type) {
    case A_Resource_RecordType:
      msg->answer->rd_length = 4;
      rc = get_A_Record(&(msg->answer->rdata), q->q_name);
      break;
    case CNAME_Resource_RecordType:        
      rc = get_CNAME_Record(&(msg->answer->rdata), q->q_name);        
      if (rc == 0) {
        msg->answer->rd_length = strlen(msg->answer->rdata) + 1;
      }
      
      break;
    case TXT_Resource_RecordType:
      rc = get_TXT_Record(&(msg->answer->rdata), q->q_name);
      if (rc == 0) {
        msg->answer->rd_length = strlen(msg->answer->rdata) + 1;
      }      
      // extra one byte for length of txt string: https://en.wikipedia.org/wiki/TXT_record
      break;
    /*
    These RecordTypes are not supported at the moment...
    case NS_Resource_RecordType:
    case CNAME_Resource_RecordType:
    case SOA_Resource_RecordType:
    case PTR_Resource_RecordType:
    case MX_Resource_RecordType:
    case TXT_Resource_RecordType:
    */
    default:
      msg->header->rcode = NotImplemented_ResponseCode;
  }
  if (rc != 0) {
    msg->header->an_count = 0;
  }
  return 0;
}

/**
 * @brief 
 * 
 * @param rr 
 * @param buffer 
 * @return int 0 means success; 1: failure
 */
int encode_resource_records(struct Message *msg, uint8_t **buffer)
{
  if (msg->header->an_count == 0) {
    return 0;
  }
  struct ResourceRecord* rr = msg->answer;
  // Answer questions by attaching resource sections.
  encode_domain_name(buffer, rr->name);
  set_and_move_by_2bytes(buffer, rr->type);
  set_and_move_by_2bytes(buffer, rr->class);
  set_and_move_by_4bytes(buffer, rr->ttl);
  set_and_move_by_2bytes(buffer, rr->rd_length);

  if (rr->type == A_Resource_RecordType || rr->type == CNAME_Resource_RecordType) {
    for (int i = 0; i < rr->rd_length; ++i) {
        set_and_move_by_1byte(buffer, rr->rdata[i]);
      }
  } else if (rr->type == TXT_Resource_RecordType) {
    set_and_move_by_1byte(buffer, rr->rd_length - 1);
    // minus one byte, which is for txt string length
    for (int i = 0; i < rr->rd_length - 1; i++) {
      set_and_move_by_1byte(buffer, rr->rdata[i]);
    }
  } else { 
      fprintf(stderr, "Unknown type %u. => Ignore resource record.\n", rr->type);
    return 1;
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

  encode_header(msg->header, buffer);

  encode_domain_name(buffer, msg->question->q_name);
  set_and_move_by_2bytes(buffer, msg->question->q_type);
  set_and_move_by_2bytes(buffer, msg->question->q_class);

  rc = encode_resource_records(msg, buffer);

  return rc;
}

void free_message(struct Message *msg) {
    free(msg->header);
    // the C standard, 7.20.3.2/2 from ISO-IEC 9899: If ptr is a null pointer, no action occurs.
    if (msg->question != NULL) {
      free(msg->question->q_name);
    }
    free(msg->question);
    if (msg->answer != NULL) {
      free_resource_records(&(msg->answer));
    }
    msg->header = NULL;
    msg->question = NULL;
    
    // setting pointers to NULL is not required by the C standard--it is a precaution to
    // avoid accessing dangling pointers.
    memset(msg, 0, sizeof(struct Message));
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
    free_message(&msg);
    nbytes = recvfrom(sock, read_buf, sizeof(read_buf), 0, (struct sockaddr *) &client_addr, &addr_len);

    int retval = parse_dns_query(&msg, read_buf, nbytes);
    if (retval != NoError_ResponseCode) {
      fprintf(stderr, "Can't handle incoming DNS query, ResponseCode == %d: ", retval);
      fprintf(stderr, "this program is not able to make a valid reply based on an invalid query, ");
      fprintf(stderr, "and thus the query is dropped silently.\n");
      continue;
    }
    printf("-----===== DNS request received =====-----\n");
    printf("          ===== Question =====\n");
    print_message(&msg);

    /* Resolve query and put the answers into the query message */
    if (resolve_query(&msg) != 0) {
      fprintf(stderr, "Failed to resolve the seemingly valid query, dropped it\n");
      continue;
    }

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
    // seems sendto() is the UDP's equivalent of TCP's send()
  }
}
