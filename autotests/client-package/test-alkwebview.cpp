#include <alkimia/alkwebpage.h>
#include <alkimia/alkwebview.h>

int testWebPage()
{
#if defined(ALKIMIA_WEBENGINE)
    AlkWebView::webInspectorEnabled();
#endif
#if defined(ALKIMIA_WEBKIT)
    AlkWebView view;
    view.webInspectorEnabled();
#endif
    return 0;
}

int main()
{
    return testWebPage();
}
