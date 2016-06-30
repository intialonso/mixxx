// recordingfeature.cpp
// Created 03/26/2010 by Tobias Rafreider

#include "library/recording/dlgrecording.h"
#include "track/track.h"
#include "library/treeitem.h"
#include "library/recording/recordingfeature.h"
#include "library/library.h"
#include "library/trackcollection.h"
#include "widget/wlibrary.h"
#include "controllers/keyboard/keyboardeventfilter.h"

const QString RecordingFeature::m_sRecordingViewName = QString("Recording");

RecordingFeature::RecordingFeature(UserSettingsPointer pConfig,
                                   Library* pLibrary,
                                   QObject* parent,
                                   TrackCollection* pTrackCollection,
                                   RecordingManager* pRecordingManager)
        : LibraryFeature(pConfig, pLibrary, parent),
          m_pTrackCollection(pTrackCollection),
          m_pRecordingManager(pRecordingManager),
          m_pRecordingView(nullptr) {
}

RecordingFeature::~RecordingFeature() {

}

QVariant RecordingFeature::title() {
    return QVariant(tr("Recordings"));
}

QIcon RecordingFeature::getIcon() {
    return QIcon(":/images/library/ic_library_recordings.png");
}

TreeItemModel* RecordingFeature::getChildModel() {
    return &m_childModel;
}

void RecordingFeature::bindPaneWidget(WLibrary* pLibraryWidget,
                                      KeyboardEventFilter *pKeyboard, int paneId) {    
    QWidget* pPane = createPaneWidget(pKeyboard, paneId);
    pPane->setParent(pLibraryWidget);
    pLibraryWidget->registerView(m_sRecordingViewName, pPane);
}

QWidget* RecordingFeature::createPaneWidget(KeyboardEventFilter* pKeyboard, int) {
    WTrackTableView* pTrackTableView = new WTrackTableView(nullptr, 
                                                           m_pConfig, 
                                                           m_pTrackCollection, 
                                                           false); // No sorting
    pTrackTableView->installEventFilter(pKeyboard);
    
    connect(m_pLibrary, SIGNAL(setTrackTableFont(QFont)),
            pTrackTableView, SLOT(setTrackTableFont(QFont)));
    connect(m_pLibrary, SIGNAL(setTrackTableRowHeight(int)),
            pTrackTableView, SLOT(setTrackTableRowHeight(int)));
    pTrackTableView->loadTrackModel(m_pProxyModel);
    
    if (m_pRecordingView) {
        m_pRecordingView->setTrackTable(pTrackTableView);
    } else {
        m_trackTables.append(pTrackTableView);
    }
    
    return pTrackTableView;
}

void RecordingFeature::bindSidebarWidget(WBaseLibrary* pBaseLibrary,
                                         KeyboardEventFilter* pKeyboard) {
    
    QWidget* pSidebar = createSidebarWidget(pKeyboard);
    pSidebar->setParent(pBaseLibrary);
    pBaseLibrary->registerView(m_sRecordingViewName, m_pRecordingView);
}

QWidget *RecordingFeature::createSidebarWidget(KeyboardEventFilter *pKeyboard) {
    m_pRecordingView = new DlgRecording(nullptr, 
                                        m_pTrackCollection,
                                        m_pRecordingManager);
    m_pRecordingView->installEventFilter(pKeyboard);
    m_pRecordingView->setBrowseTableModel(getBrowseTableModel());
    m_pRecordingView->setProxyTrackModel(getProxyTrackModel());
    
    return m_pRecordingView;
}


void RecordingFeature::activate() {
    m_pRecordingView->refreshBrowseModel();
    emit(switchToView(m_sRecordingViewName));
    emit(restoreSearch(""));
    emit(enableCoverArtDisplay(false));
}

BrowseTableModel* RecordingFeature::getBrowseTableModel() {
    if (!m_pBrowseModel) {
        m_pBrowseModel = new BrowseTableModel(this, m_pTrackCollection, m_pRecordingManager);
    }
    
    return m_pBrowseModel;
}

ProxyTrackModel* RecordingFeature::getProxyTrackModel() {
    if (!m_pProxyModel) {
        m_pProxyModel = new ProxyTrackModel(getBrowseTableModel());
    }
    
    return m_pProxyModel;
}
