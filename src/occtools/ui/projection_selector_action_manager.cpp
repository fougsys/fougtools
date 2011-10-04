/****************************************************************************
**
**  FougTools
**  Copyright FougSys (1 Mar. 2011)
**  contact@fougsys.fr
**
** This software is a computer program whose purpose is to provide utility
** tools for the C++ language, the Qt and Open Cascade toolkits.
**
** This software is governed by the CeCILL-C license under French law and
** abiding by the rules of distribution of free software.  You can  use,
** modify and/ or redistribute the software under the terms of the CeCILL-C
** license as circulated by CEA, CNRS and INRIA at the following URL
** "http://www.cecill.info".
**
** As a counterpart to the access to the source code and  rights to copy,
** modify and redistribute granted by the license, users are provided only
** with a limited warranty  and the software's author,  the holder of the
** economic rights,  and the successive licensors  have only  limited
** liability.
**
** In this respect, the user's attention is drawn to the risks associated
** with loading,  using,  modifying and/or developing or reproducing the
** software by the user in light of its specific status of free software,
** that may mean  that it is complicated to manipulate,  and  that  also
** therefore means  that it is reserved for developers  and  experienced
** professionals having in-depth computer knowledge. Users are therefore
** encouraged to load and test the software's suitability as regards their
** requirements in conditions enabling the security of their systems and/or
** data to be ensured and,  more generally, to use and operate it in the
** same conditions as regards security.
**
** The fact that you are presently reading this means that you have had
** knowledge of the CeCILL-C license and that you accept its terms.
**
****************************************************************************/

#include "occtools/ui/projection_selector_action_manager.h"

#include <cassert>
#include <QtCore/QHash>
#include <QtCore/QSignalMapper>
#include <QtGui/QAction>
#include <QtGui/QMenu>
#include <QtGui/QToolButton>
#include <QtGui/QVBoxLayout>

namespace occ {

ProjectionSelectorActionManager::ProjectionSelectorActionManager(QObject* parent) :
  QObject(parent),
  _mainAction(0),
  _currOrientation(V3d_XposYnegZpos),
  _signalMapper(0)
{
  _signalMapper = new QSignalMapper(this);
  QMenu* menu = new QMenu(tr("View projection"));
  QHash<V3d_TypeOfOrientation, QAction*> actions;
  actions[V3d_Xpos] = menu->addAction(QIcon(":/images/view_front.png"),
                                      tr("Front"));
  actions[V3d_Xneg] = menu->addAction(QIcon(":/images/view_back.png"),
                                      tr("Back"));
  actions[V3d_Zpos] = menu->addAction(QIcon(":/images/view_top.png"),
                                      tr("Top"));
  actions[V3d_Zneg] = menu->addAction(QIcon(":/images/view_bottom.png"),
                                      tr("Bottom"));
  actions[V3d_Ypos] = menu->addAction(QIcon(":/images/view_left.png"),
                                      tr("Left"));
  actions[V3d_Yneg] = menu->addAction(QIcon(":/images/view_right.png"),
                                      tr("Right"));
  actions[V3d_XposYnegZpos] = menu->addAction(QIcon(":/images/view_axo.png"),
                                              tr("Axonometric"));
  foreach(V3d_TypeOfOrientation proj, actions.keys()) {
    connect(actions[proj], SIGNAL(triggered()),
            _signalMapper, SLOT(map()));
    _signalMapper->setMapping(actions[proj], static_cast<int>(proj));
  }
  connect(_signalMapper, SIGNAL(mapped(int)),
          this, SLOT(onViewProjActionTriggered(int)));
  _mainAction = new QAction(this);
  _mainAction->setMenu(menu);
  connect(_mainAction, SIGNAL(triggered(bool)),
          this, SLOT(onMainActionTriggered(bool)));
}

// --- Access

QAction* ProjectionSelectorActionManager::mainAction()
{
  return _mainAction;
}

// --- Element change

void ProjectionSelectorActionManager::triggerViewProjection(V3d_TypeOfOrientation proj)
{
  QAction* action = this->mappedAction(static_cast<int>(proj));
  _mainAction->setIcon(action->icon());
  _currOrientation = proj;
}

// --- Qt slots

void ProjectionSelectorActionManager::onMainActionTriggered(bool /*checked*/)
{
  emit viewProjectionTriggered(_currOrientation);
}

void ProjectionSelectorActionManager::onViewProjActionTriggered(int projId)
{
  QAction* action = this->mappedAction(projId);
  assert(action != 0 && "sender_is_a_qaction");
  _mainAction->setIcon(action->icon());
  V3d_TypeOfOrientation ori = static_cast<V3d_TypeOfOrientation>(projId);
  _currOrientation = ori;
  emit viewProjectionTriggered(ori);
}

// --- Implementation

QAction* ProjectionSelectorActionManager::mappedAction(int projId) const
{
  return qobject_cast<QAction*>(_signalMapper->mapping(projId));
}

} // namespace occ
