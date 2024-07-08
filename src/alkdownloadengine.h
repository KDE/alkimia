/*
    SPDX-FileCopyrightText: 2024 Ralf Habacker ralf.habacker @freenet.de

    This file is part of libalkimia.

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef ALKDOWNLOADENGINE_H
#define ALKDOWNLOADENGINE_H

#include <alkimia/alk_export.h>

#include <QMetaType>
#include <QObject>
#include <QUrl>

class AlkWebPage;

/**
* The DownloadEngine class provides a general interface for
* the supported implementation for downloading page content
 */
class ALK_EXPORT AlkDownloadEngine : public QObject
{
    Q_OBJECT
public:
    enum Type {
        QtEngine,
        WebKitEngine,
        WebEngine,
        DefaultEngine, ///< Use t network support
        JavaScriptEngine, ///< Use an HTML engine with Javascript support
        JavaScriptEngineCSS, ///< Use an HTML engine with Javascript support, @see AlkDownloadEngine::finishedPage for details
    };

    /**
     * Constructor
     * @param parent Parent object
     */
    explicit AlkDownloadEngine(QObject *parent = nullptr);

    /**
     * Destructor
     */
    ~AlkDownloadEngine();

    /**
     * Specifies a custom instance of the HTML engine that is used to download content.
     * If this method is not called, an internal instance is used.
     * @param webPage Instance of HTML engine
     */
    void setWebPage(AlkWebPage *webPage);

    /**
     * Download content from a URL using the engine specified by the \p type.
     * This method blocks until the download is complete. The status and result
     * of the download is returned via the available signals.
     * @param url Url to download
     * @param type Type of engine to use for downloading
     * @param timeout Timeout for fetching the content [msec]
     * @return true - fetching succeded
     * @return false - fetching failed
     */
    bool downloadUrl(const QUrl &url, Type type);

    /**
     * Set specific language to be accepted for the download.
     * The supported format of the specified language is described at
     * https://www.rfc-editor.org/rfc/rfc9110.html#field.accept-language.
     * @param language String with the accepted language
     */
    void setAcceptedLanguage(const QString &language);

    /**
     * Set timeout for download operation
     * A \p timeout value of -1 disables timeout support
     * @param timeout Timeout in milliseconds
     */
    void setTimeout(int timeout = -1);

    /**
     * Return timeout for download operation
     * @return timeout in milliseconds
     */
    int timeout();

Q_SIGNALS:
    /**
     * Emitted in case of of errors
     * @param url The url from which the error has been recognzied
     * @param message Error message
     */
    void error(const QUrl &url, const QString &message);

    /**
     * Is emitted if the type parameter of AlkDownloadEngine::downloadUrl()
     * is AlkDownloadEngine::DefaultEngine or AlkDownloadEngine::JavaScriptEngine.
     * @param url The url from which the data is returned
     * @param data The returned data
     */
    void finished(const QUrl &url, const QString &data);

    /**
     * Is emitted if the type parameter of AlkDownloadEngine::downloadUrl()
     * is JavaScriptEngineCSS.
     * @param url The url from which the data is returned
     * @param page The page instance containing the returned data
     */
    void finishedPage(const QUrl &url, AlkWebPage *page);

    /**
     * Is emitted if the download has been started
     * @param url The associated url
     */
    void started(const QUrl &url);

    /**
     * Is emitted if the url has been redirected
     * @param url The associated url
     * @param newurl The redirected url
     */
    void redirected(const QUrl &url, const QUrl &newUrl);

    /**
     * Is emitted if the download request has been timed out
     * @param url The url which has been timeout out
     */
    void timeout(const QUrl &url);

private:
    class Private;
    Private *d;
};

Q_DECLARE_METATYPE(AlkDownloadEngine::Type)

#endif // ALKDOWNLOADENGINE_H
