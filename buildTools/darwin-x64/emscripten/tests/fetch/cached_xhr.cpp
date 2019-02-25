#include <string.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <emscripten/fetch.h>

int result = 0;

// Fetch file without XHRing.
void fetchFromIndexedDB()
{
  emscripten_fetch_attr_t attr;
  emscripten_fetch_attr_init(&attr);
  strcpy(attr.requestMethod, "GET");
  attr.onsuccess = [](emscripten_fetch_t *fetch) {
    assert(fetch->numBytes == fetch->totalBytes);
    assert(fetch->data != 0);
    printf("Finished downloading %llu bytes\n", fetch->numBytes);
    emscripten_fetch_close(fetch);

#ifdef REPORT_RESULT
    REPORT_RESULT(1);
#endif

  };
  attr.onprogress = [](emscripten_fetch_t *fetch) {
    if (fetch->totalBytes > 0) {
      printf("Downloading.. %.2f%% complete.\n", (fetch->dataOffset + fetch->numBytes) * 100.0 / fetch->totalBytes);
    } else {
      printf("Downloading.. %lld bytes complete.\n", fetch->dataOffset + fetch->numBytes);
    }
  };
  attr.attributes = EMSCRIPTEN_FETCH_APPEND | EMSCRIPTEN_FETCH_NO_DOWNLOAD;
  emscripten_fetch_t *fetch = emscripten_fetch(&attr, "gears.png");
}

// XHR and store to cache.
int main()
{
  emscripten_fetch_attr_t attr;
  emscripten_fetch_attr_init(&attr);
  strcpy(attr.requestMethod, "GET");
  attr.onsuccess = [](emscripten_fetch_t *fetch) {
    assert(fetch->numBytes == fetch->totalBytes);
    assert(fetch->data != 0);
    printf("Finished downloading %llu bytes\n", fetch->numBytes);
    emscripten_fetch_close(fetch);

    // Test that the file now exists:
    fetchFromIndexedDB();
  };
  attr.onprogress = [](emscripten_fetch_t *fetch) {
    if (fetch->totalBytes > 0) {
      printf("Downloading.. %.2f%% complete.\n", (fetch->dataOffset + fetch->numBytes) * 100.0 / fetch->totalBytes);
    } else {
      printf("Downloading.. %lld bytes complete.\n", fetch->dataOffset + fetch->numBytes);
    }
  };
  attr.attributes = EMSCRIPTEN_FETCH_REPLACE | EMSCRIPTEN_FETCH_LOAD_TO_MEMORY | EMSCRIPTEN_FETCH_PERSIST_FILE;
  emscripten_fetch_t *fetch = emscripten_fetch(&attr, "gears.png");
  assert(fetch != 0);
  memset(&attr, 0, sizeof(attr)); // emscripten_fetch() must be able to operate without referencing to this structure after the call.
}