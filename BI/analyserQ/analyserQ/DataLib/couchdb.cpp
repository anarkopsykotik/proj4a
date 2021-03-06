#include "couchdb.h"
#include "couchdbserver.h"
#include "couchdbquery.h"
#include "couchdblistener.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QtQml>
#include <QDebug>


class CouchDBPrivate
{
public:
    CouchDBPrivate() :
        server(0),
        cleanServerOnQuit(true),
        networkManager(0)
    {}
    
    virtual ~CouchDBPrivate()
    {
        if(server && cleanServerOnQuit) delete server;
        
        if(networkManager) delete networkManager;
    }
    
    CouchDBServer *server;
    bool cleanServerOnQuit;

    QNetworkAccessManager *networkManager;
    QHash<QNetworkReply*, CouchDBQuery*> currentQueries;
};

CouchDB::CouchDB(QObject *parent) :
    QObject(parent),
    d_ptr(new CouchDBPrivate)
{
    Q_D(CouchDB);

    d->server = new CouchDBServer(this);
    d->networkManager = new QNetworkAccessManager(this);
}

CouchDB::~CouchDB()
{
    delete d_ptr;
}

void CouchDB::declareQML()
{
    qmlRegisterType<CouchDB>("TOP.CouchDB", 1, 0, "CouchDB");
}

CouchDBServer *CouchDB::server() const
{
    Q_D(const CouchDB);
    return d->server;
}

void CouchDB::setServer(CouchDBServer *server)
{
    Q_D(CouchDB);
    if(d->server && d->cleanServerOnQuit) delete d->server;

    d->server = server;
    d->cleanServerOnQuit = false;
}

void CouchDB::setServerConfiguration(const QString &url, const int &port, const QString &username, const QString &password)
{
    Q_D(CouchDB);
    d->server->setUrl(url);
    d->server->setPort(port);
    if(!username.isEmpty() && !password.isEmpty()) d->server->setCredential(username, password);
}

void CouchDB::executeQuery(CouchDBQuery *query)
{
    Q_D(CouchDB);

    if(query->server()->hasCredential() && query->operation() != COUCHDB_STARTSESSION)
    {
        query->request()->setRawHeader("Authorization", "Basic " + query->server()->credential());
    }

    qDebug() << "Invoked url:" << query->operation() << query->request()->url().toString();

    QNetworkReply * reply;
    switch(query->operation()) {
    case COUCHDB_CHECKINSTALLATION:
    default:
        reply = d->networkManager->get(*query->request());
        break;
    case COUCHDB_STARTSESSION:
        reply = d->networkManager->post(*query->request(), query->body());
        break;
    case COUCHDB_ENDSESSION:
        reply = d->networkManager->deleteResource(*query->request());
        break;
    case COUCHDB_LISTDATABASES:
        reply = d->networkManager->get(*query->request());
        break;
    case COUCHDB_CREATEDATABASE:
        reply = d->networkManager->put(*query->request(), query->body());
        break;
    case COUCHDB_DELETEDATABASE:
        reply = d->networkManager->deleteResource(*query->request());
        break;
    case COUCHDB_LISTDOCUMENTS:
        reply = d->networkManager->get(*query->request());
        break;
    case COUCHDB_RETRIEVEREVISION:
        reply = d->networkManager->head(*query->request());
        break;
    case COUCHDB_RETRIEVEDOCUMENT:
        reply = d->networkManager->get(*query->request());
        break;
    case COUCHDB_UPDATEDOCUMENT:
        reply = d->networkManager->put(*query->request(), query->body());
        break;
    case COUCHDB_DELETEDOCUMENT:
        reply = d->networkManager->deleteResource(*query->request());
        break;
    case COUCHDB_UPLOADATTACHMENT:
        reply = d->networkManager->put(*query->request(), query->body());
        break;
    case COUCHDB_DELETEATTACHMENT:
        reply = d->networkManager->deleteResource(*query->request());
        break;
    case COUCHDB_REPLICATEDATABASE:
        reply = d->networkManager->post(*query->request(), query->body());
        break;
    }

    if(query->operation() != COUCHDB_REPLICATEDATABASE)
    {
        connect(query, SIGNAL(timeout()), SLOT(queryTimeout()));
        query->startTimeoutTimer();
    }

    connect(reply, SIGNAL(finished()), this, SLOT(queryFinished()));
    d->currentQueries[reply] = query;
}

void CouchDB::queryFinished()
{
    Q_D(CouchDB);

    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if(!reply) return;

    QByteArray data;
    CouchDBQuery *query = d->currentQueries[reply];
    bool hasError = false;
    if(reply->error() == QNetworkReply::NoError)
    {
        data = reply->readAll();
    }
    else
    {
        qWarning() << reply->errorString();
        hasError = true;
    }

    CouchDBResponse response;
    response.setQuery(query);
    response.setData(data);
    response.setStatus(hasError || (query->operation() != COUCHDB_CHECKINSTALLATION && query->operation() != COUCHDB_RETRIEVEDOCUMENT &&
            !response.documentObj().value("ok").toBool()) ? COUCHDB_ERROR : COUCHDB_SUCCESS);

    switch(query->operation())
    {
    case COUCHDB_CHECKINSTALLATION:
    default:
        if(!hasError) response.setStatus(response.documentObj().contains("couchdb") ? COUCHDB_SUCCESS : COUCHDB_ERROR);
        emit installationChecked(response);
        break;
    case COUCHDB_STARTSESSION:
        if(hasError && reply->error() >= 201 && reply->error() <= 299) response.setStatus(COUCHDB_AUTHERROR);
        emit sessionStarted(response);
        break;
    case COUCHDB_ENDSESSION:
        emit sessionEnded(response);
        break;
    case COUCHDB_LISTDATABASES:
        emit databasesListed(response);
        break;
    case COUCHDB_CREATEDATABASE:
        emit databaseCreated(response);
        break;
    case COUCHDB_DELETEDATABASE:
        emit databaseDeleted(response);
        break;
    case COUCHDB_LISTDOCUMENTS:
        emit documentsListed(response);
        break;
    case COUCHDB_RETRIEVEREVISION:
    {
        QString revision = reply->rawHeader("ETag");
        revision.remove("\"");
        response.setRevisionData(revision);
        emit revisionRetrieved(response);
        break;
    }
    case COUCHDB_RETRIEVEDOCUMENT:
        emit documentRetrieved(response);
        break;
    case COUCHDB_UPDATEDOCUMENT:
        emit documentUpdated(response);
        break;
    case COUCHDB_DELETEDOCUMENT:
        emit documentDeleted(response);
        break;
    case COUCHDB_UPLOADATTACHMENT:
        emit attachmentUploaded(response);
        break;
    case COUCHDB_DELETEATTACHMENT:
        emit attachmentDeleted(response);
        break;
    case COUCHDB_REPLICATEDATABASE:
        emit databaseReplicated(response);
        break;
    }

    d->currentQueries.remove(reply);
    reply->deleteLater();
    delete query;
}

void CouchDB::queryTimeout()
{
    CouchDBQuery *query = qobject_cast<CouchDBQuery*>(sender());
    if(!query) return;

    qWarning() << query->url() << "timed out. Retrying...";

    executeQuery(query);
}

void CouchDB::checkInstallation()
{
    Q_D(CouchDB);

    CouchDBQuery *query = new CouchDBQuery(d->server, this);
    query->setUrl(QString("%1").arg(d->server->baseURL()));
    query->setOperation(COUCHDB_CHECKINSTALLATION);

    executeQuery(query);
}

void CouchDB::startSession(const QString &username, const QString &password)
{
    Q_D(CouchDB);

    QUrlQuery postData;
    postData.addQueryItem("name", username);
    postData.addQueryItem("password", password);

    CouchDBQuery *query = new CouchDBQuery(d->server, this);
    query->setUrl(QString("%1/_session").arg(d->server->baseURL()));
    query->setOperation(COUCHDB_STARTSESSION);
    query->request()->setRawHeader("Accept", "application/json");
    query->request()->setRawHeader("Content-Type", "application/x-www-form-urlencoded");
    query->setBody(postData.toString(QUrl::FullyEncoded).toUtf8());

    executeQuery(query);
}

void CouchDB::endSession()
{
    Q_D(CouchDB);

    CouchDBQuery *query = new CouchDBQuery(d->server, this);
    query->setUrl(QString("%1/_session").arg(d->server->baseURL()));
    query->setOperation(COUCHDB_ENDSESSION);

    executeQuery(query);
}

void CouchDB::listDatabases()
{
    Q_D(CouchDB);

    CouchDBQuery *query = new CouchDBQuery(d->server, this);
    query->setUrl(QString("%1/_all_dbs").arg(d->server->baseURL()));
    query->setOperation(COUCHDB_LISTDATABASES);

    executeQuery(query);
}

void CouchDB::createDatabase(const QString &database)
{
    Q_D(CouchDB);

    CouchDBQuery *query = new CouchDBQuery(d->server, this);
    query->setUrl(QString("%1/%2").arg(d->server->baseURL(), database));
    query->setOperation(COUCHDB_CREATEDATABASE);
    query->setDatabase(database);

    executeQuery(query);
}

void CouchDB::deleteDatabase(const QString &database)
{
    Q_D(CouchDB);

    CouchDBQuery *query = new CouchDBQuery(d->server, this);
    query->setUrl(QString("%1/%2").arg(d->server->baseURL(), database));
    query->setOperation(COUCHDB_DELETEDATABASE);
    query->setDatabase(database);

    executeQuery(query);
}

void CouchDB::listDocuments(const QString& database)
{
    Q_D(CouchDB);

    CouchDBQuery *query = new CouchDBQuery(d->server, this);
    query->setUrl(QString("%1/%2/_all_docs").arg(d->server->baseURL(), database));
    query->setOperation(COUCHDB_LISTDOCUMENTS);
    query->setDatabase(database);

    executeQuery(query);
}

void CouchDB::retrieveRevision(const QString &database, const QString &id)
{
    Q_D(CouchDB);

    CouchDBQuery *query = new CouchDBQuery(d->server, this);
    query->setUrl(QString("%1/%2/%3").arg(d->server->baseURL(), database, id));
    query->setOperation(COUCHDB_RETRIEVEREVISION);
    query->setDatabase(database);
    query->setDocumentID(id);

    executeQuery(query);
}

void CouchDB::retrieveDocument(const QString &database, const QString &id)
{
    Q_D(CouchDB);

    CouchDBQuery *query = new CouchDBQuery(d->server, this);
    query->setUrl(QString("%1/%2/%3").arg(d->server->baseURL(), database, id));
    query->setOperation(COUCHDB_RETRIEVEDOCUMENT);
    query->setDatabase(database);
    query->setDocumentID(id);

    executeQuery(query);
}

void CouchDB::updateDocument(const QString &database, const QString &id, QByteArray document)
{
    Q_D(CouchDB);

    QByteArray postDataSize = QByteArray::number(document.size());

    CouchDBQuery *query = new CouchDBQuery(d->server, this);
    query->setUrl(QString("%1/%2/%3").arg(d->server->baseURL(), database, id));
    query->setOperation(COUCHDB_UPDATEDOCUMENT);
    query->setDatabase(database);
    query->setDocumentID(id);
    query->request()->setRawHeader("Accept", "application/json");
    query->request()->setRawHeader("Content-Type", "application/json");
    query->request()->setRawHeader("Content-Length", postDataSize);
    query->setBody(document);

    executeQuery(query);
}

void CouchDB::deleteDocument(const QString &database, const QString &id, const QString &revision)
{
    Q_D(CouchDB);

    CouchDBQuery *query = new CouchDBQuery(d->server, this);
    query->setUrl(QString("%1/%2/%3?rev=%4").arg(d->server->baseURL(), database, id, revision));
    query->setOperation(COUCHDB_DELETEDOCUMENT);
    query->setDatabase(database);
    query->setDocumentID(id);

    executeQuery(query);
}

void CouchDB::uploadAttachment(const QString &database, const QString &id, const QString& attachmentName,
                               QByteArray attachment, QString mimeType, const QString& revision)
{
    Q_D(CouchDB);

    QByteArray postDataSize = QByteArray::number(attachment.size());

    CouchDBQuery *query = new CouchDBQuery(d->server, this);
    query->setUrl(QString("%1/%2/%3/%4?rev=%5").arg(d->server->baseURL(), database, id, attachmentName, revision));
    query->setOperation(COUCHDB_DELETEDOCUMENT);
    query->setDatabase(database);
    query->setDocumentID(id);
    query->request()->setRawHeader("Content-Type", mimeType.toLatin1());
    query->request()->setRawHeader("Content-Length", postDataSize);
    query->setBody(attachment);

    executeQuery(query);
}

void CouchDB::deleteAttachment(const QString &database, const QString &id, const QString &attachmentName, const QString &revision)
{
    Q_D(CouchDB);

    CouchDBQuery *query = new CouchDBQuery(d->server, this);
    query->setUrl(QString("%1/%2/%3/%4?rev=%5").arg(d->server->baseURL(), database, id, attachmentName, revision));
    query->setOperation(COUCHDB_DELETEATTACHMENT);
    query->setDatabase(database);
    query->setDocumentID(id);

    executeQuery(query);
}

void CouchDB::replicateDatabaseFrom(CouchDBServer *sourceServer, const QString& sourceDatabase, const QString& targetDatabase,
                                    const bool& createTarget, const bool& continuous, const bool& cancel)
{
    Q_D(CouchDB);

    QString source = QString("%1/%2").arg(sourceServer->baseURL(true), sourceDatabase);
    QString target = d->server->url().contains("localhost") ? targetDatabase : QString("%1/%2").arg(d->server->baseURL(true), targetDatabase);

    replicateDatabase(source, target, targetDatabase, createTarget, continuous, cancel);
}

void CouchDB::replicateDatabaseTo(CouchDBServer *targetServer, const QString& sourceDatabase, const QString& targetDatabase,
                                    const bool& createTarget, const bool& continuous, const bool& cancel)
{
    Q_D(CouchDB);

    QString source = d->server->url().contains("localhost") ? sourceDatabase : QString("%1/%2").arg(d->server->baseURL(true), sourceDatabase);
    QString target = QString("%1/%2").arg(targetServer->baseURL(true), targetDatabase);

    replicateDatabase(source, target, targetDatabase, createTarget, continuous, cancel);
}

void CouchDB::replicateDatabase(const QString &source, const QString &target, const QString& database, const bool &createTarget,
                                const bool &continuous, const bool &cancel)
{
    Q_D(CouchDB);

    if(!cancel) qDebug() << "Starting replication from" << source << "to" << target;
    else qDebug() << "Cancelling replication from" << source << "to" << target;

    QJsonObject object;
    object.insert("source", source);
    object.insert("target", target);
    object.insert("create_target", createTarget);
    object.insert("continuous", continuous);
    object.insert("cancel", cancel);
    QJsonDocument document(object);

    CouchDBQuery *query = new CouchDBQuery(d->server, this);
    query->setUrl(QString("%1/_replicate").arg(d->server->baseURL(true)));
    query->setOperation(COUCHDB_REPLICATEDATABASE);
    query->setDatabase(database);
    query->request()->setRawHeader("Accept", "application/json");
    query->request()->setRawHeader("Content-Type", "application/json");
    query->setBody(document.toJson());

    executeQuery(query);
}

CouchDBListener* CouchDB::createListener(const QString &database, const QString &documentID)
{
    Q_D(CouchDB);

    CouchDBListener *listener = new CouchDBListener(d->server);
    listener->setCookieJar(d->networkManager->cookieJar());
    d->networkManager->cookieJar()->setParent(0);
    listener->setDatabase(database);
    listener->setDocumentID(documentID);
    listener->launch();

    qDebug() << "Created listener for database:" << database << ", document:" << documentID;

    return listener;
}
