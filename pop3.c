
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include <stdio.h>
#include <curl/curl.h>
 
/* This is a simple example showing how to fetch mail using libcurl's IMAP
 * capabilities.
 *
 * Note that this example requires libcurl 7.30.0 or above.
 */ 
 static size_t header_handler(void *ptr, size_t size,
                             size_t nmemb, void *userdata) {
    char *x = calloc(size + 1, nmemb);
    assert(x);
    memcpy(x, ptr, size * nmemb);
    printf("New header:	%s", x);
		free(x);
    return size * nmemb;
}


static size_t write_callback(char *buffer,
                             size_t size,
                             size_t nitems,
                             void *userp)
{

	printf("\nsize: %d, nitems %d\n",size,nitems);
	int i=0;
	for(i=0;i<nitems;i++){
		printf("%c",*(buffer+i));
	}
	//printf("/size: %d, nitems %d\n",size,nitems);

	return size*nitems;

}

int main(void)
{
  CURL *curl;
  CURLcode res = CURLE_OK;
 
  curl = curl_easy_init();
  if(curl) {
    /* Set username and password */ 
    curl_easy_setopt(curl, CURLOPT_USERNAME, "homecontrol@126.com");
    curl_easy_setopt(curl, CURLOPT_PASSWORD, "--------");
 		//curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "TOP 2 0");
    /* This will fetch message 1 from the user's inbox */ 
    //curl_easy_setopt(curl, CURLOPT_URL, "imap://imap.163.com/INBOX/");
    curl_easy_setopt(curl, CURLOPT_URL, "pop3://pop.126.com/3");
 		curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, header_handler);
 		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);


    /* Perform the fetch */ 
    res = curl_easy_perform(curl);
 
    /* Check for errors */ 
    if(res != CURLE_OK)
      fprintf(stderr, "curl_easy_perform() failed: %s\n",
              curl_easy_strerror(res));
 
    /* Always cleanup */ 
    curl_easy_cleanup(curl);
  }
 
  return (int)res;
}

