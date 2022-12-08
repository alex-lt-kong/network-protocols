#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <curl/curl.h>

struct MemoryStruct {
  char *memory;
  size_t size;
};

static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
  size_t realsize = size * nmemb;
  struct MemoryStruct *mem = (struct MemoryStruct *)userp;

  char *ptr = realloc(mem->memory, mem->size + realsize + 1);
  if(!ptr) {
    /* out of memory! */
    fprintf(stderr, "not enough memory (realloc returned NULL)\n");
    return 0;
  }

  mem->memory = ptr;
  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;
  
  return realsize;
}


int main(void)
{
  CURL *curl;
  CURLcode res;
  struct MemoryStruct resp_body;
  struct MemoryStruct resp_header;
  
  resp_body.memory = malloc(1);  /* will be grown as needed by realloc above */
  resp_body.size = 0;    /* no data at this point */
  resp_header.memory = malloc(1);  /* will be grown as needed by realloc above */
  resp_header.size = 0;    /* no data at this point */
  char *post_this = getenv("CURL_TEST_POST_THIS");
  // https://docs.rundeck.com/docs/api/rundeck-api.html#password-authentication
  // should be something like "j_username=admin&j_password=admin"

  curl_global_init(CURL_GLOBAL_ALL);
  curl = curl_easy_init();
  
  if(curl) {
    /* First set the URL that is about to receive our POST. This URL can
       just as well be an https:// URL if that is what should receive the
       data. */
    curl_easy_setopt(curl, CURLOPT_URL, getenv("CURL_TEST_RUNDECK_J_SECURITY_CHECK_URL"));
    // $RUNDECK_SERVER_URL/j_security_check e.g.,: http://localhost:4440/j_security_check
    /* Now specify the POST data */
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_this);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&resp_body);
    curl_easy_setopt(curl, CURLOPT_WRITEHEADER, WriteMemoryCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEHEADER, (void *)&resp_header);
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    // This will make libcurl print a lot of information, but CURLOPT_POSTFIELDS won't be among it.
    // To examine the complete POST request in bytes, it seems to be easier to use another tool such as Wireshark.
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");
    /* Perform the request, res will get the return code */
    res = curl_easy_perform(curl);
    /* Check for errors */
    if(res != CURLE_OK) {
      fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
    } else {
      printf("=====resp_body.memory=====\n%s\n=====resp_body.memory=====\n", resp_body.memory);
      printf("\n");
      printf("=====resp_header.memory=====\n%s\n=====resp_header.memory=====\n", resp_header.memory);
    }
    /* always cleanup */
    curl_easy_cleanup(curl);
  }
  curl_global_cleanup();
  return 0;
}