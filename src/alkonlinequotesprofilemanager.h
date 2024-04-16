/*
    SPDX-FileCopyrightText: 2018 Ralf Habacker ralf.habacker @freenet.de

    This file is part of libalkimia.

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef ALKONLINEQUOTESPROFILEMANAGER_H
#define ALKONLINEQUOTESPROFILEMANAGER_H

#include <alkimia/alkonlinequotesprofile.h>

#include <QList>
#include <QStringList>

class AlkWebPage;
class AlkWebView;

typedef QList<AlkOnlineQuotesProfile *> AlkOnlineQuotesProfileList;

class ALK_EXPORT AlkOnlineQuotesProfileManager : public QObject
{
    Q_OBJECT
public:
    ~AlkOnlineQuotesProfileManager();

    void addProfile(AlkOnlineQuotesProfile *profile);
    AlkOnlineQuotesProfileList profiles();
    AlkOnlineQuotesProfile *profile(const QString &name);
    QStringList profileNames();

    /**
     * Set shared web page instance
     * @param webPage instance to share
     */
    void setWebPage(AlkWebPage *webPage);

    /**
     * Return shared web page instance
     * @return instance
     */
    AlkWebPage *webPage();

    /**
     * Set shared web page instance
     * @param webView instance to share
     */
    void setWebView(AlkWebView *webView);

    /**
     * Return shared web view instance
     * @return instance
     */
    AlkWebView *webView();

    /**
     * Set the status whether the web browser should be displayed
     * @param enable state to be set
     */
    void setWebViewEnabled(bool enable);

    /**
     * Return status whether the web browser should be displayed
     * @return state
     */
    bool webViewEnabled();

    /**
     * Return instance of this class
     * @return instance
     */
    static AlkOnlineQuotesProfileManager &instance();

    /**
     * Return pointer to the singleton instance of this class
     * @return pointer to instance
     */
    static AlkOnlineQuotesProfileManager *instancePointer();

Q_SIGNALS:
    /**
     * emit quote source for which an update is available
     */
    void updateAvailable(const QString &profile, const QString &name);

private:
    AlkOnlineQuotesProfileManager();
    class Private;
    Private *const d;
};

#endif // ALKONLINEQUOTESPROFILEMANAGER_H
