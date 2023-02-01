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

  curl_global_init(CURL_GLOBAL_ALL);
  curl = curl_easy_init();
  
  if(curl) {
    /* First set the URL that is about to receive our POST. This URL can
       just as well be an https:// URL if that is what should receive the
       data. */
    curl_easy_setopt(curl, CURLOPT_URL, getenv("CURL_TEST_RUNDECK_JOB_URL"));
    // This URL is more difficult to construct. It should be something like:
    // http://localhost:4440/api/1/job/<JOB_UUID>/run?argString=<ARGUMENTS>
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&resp_body);
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEHEADER, (void *)&resp_header);
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

    struct curl_slist *list = NULL;
    list = curl_slist_append(list, "Cookie: JSESSIONID=node01rlawnq2ilur7qr2o9ifxmf91670.node0;Path=/;HttpOnly");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);
    /* some servers do not like requests that are made without a user-agent
       field, so we provide one */
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");

    res = curl_easy_perform(curl);
    curl_slist_free_all(list);

    if(res != CURLE_OK)
      fprintf(stderr, "curl_easy_perform() failed: %s\n",
              curl_easy_strerror(res));
    else {
      printf("=====resp_body.memory=====\n%s\n=====resp_body.memory=====\n",resp_body.memory);
      printf("\n\n");
      printf("=====resp_header.memory=====\n%s\n=====resp_header.memory=====\n",resp_header.memory);
    }
    /* always cleanup */
    curl_easy_cleanup(curl);
  }
  free(resp_body.memory);
  free(resp_header.memory);
  curl_global_cleanup();
  return 0;
}