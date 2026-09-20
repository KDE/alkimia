/*
    SPDX-FileCopyrightText: 2024 Ralf Habacker ralf.habacker @freenet.de

    This file is part of libalkimia.

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef ALKNEWSTUFFENGINE_H
#define ALKNEWSTUFFENGINE_H

#include <alkimia/alk_export.h>
#include <alkimia/alknewstuffentry.h>

#include <QList>
#include <QObject>
#include <QStringList>

/**
 * Platform independent wrapper for the new stuff engine
 *
 * @author Ralf Habacker
 */
class ALK_EXPORT AlkNewStuffEngine : public QObject
{
    Q_OBJECT
public:
    explicit AlkNewStuffEngine(QObject *parent = nullptr);
    /**
     * Initialization of the new stuff engine
     * @param configFile path to the configuration file (.knsrc)
     * @return true - the engine was initialized
     * @return false - the engine could not be initialized
     */
    bool init(const QString &configFile);

    /**
     * Start check for updates and return
     * Update results are obtained using the updatesAvailable() signal.
     */
    void checkForUpdates();

    /**
     * @return list of available entries
     */
    AlkNewStuffEntryList availableEntries() const;

    /**
     * @return list of installed entries
     */
    AlkNewStuffEntryList installedEntries() const;

    /**
     * Reload installed entries
     */
    void reload();

    /**
     * Install the entry identified by @p entry
     * @return true - install succeeded
     * @return false - install failed
     */
    bool install(const AlkNewStuffEntry &entry);

    /**
     * Uninstall the entry identified by @p entry
     * @return true - uninstall succeeded
     * @return false - uninstall failed
     */
    bool uninstall(const AlkNewStuffEntry &entry);

    /**
     * Set provider id of an entry identified by @p name
     * @param name the name of the entry
     * @param providerId the provider id to set
     * @note This function is used as workaround for a limitation in knewstuff using the baseurl as id
     */
    void setProviderId(const QString &name, const QString &providerId);

Q_SIGNALS:
    void updatesAvailable(const AlkNewStuffEntryList &entries);
    void entriesAvailable(const AlkNewStuffEntryList &entries);

private:
    class Private;
    Private *const d;
};

#endif // ALKNEWSTUFFENGINE_H
