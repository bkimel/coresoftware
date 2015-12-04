#include "PHG4TruthInfoContainer.h"
#include "PHG4Particlev1.h"
#include "PHG4VtxPointv1.h"

#include <phool/phool.h>

#include <boost/foreach.hpp>
#include <boost/tuple/tuple.hpp>

#include <algorithm>
#include <stdexcept>
#include <cstdlib>

using namespace std;

PHG4TruthInfoContainer::PHG4TruthInfoContainer() :
  particlemap(),
  vtxmap(),
  particle_embed_flags(),
  vertex_embed_flags() {
}

PHG4TruthInfoContainer::~PHG4TruthInfoContainer() {}

void PHG4TruthInfoContainer::Reset() {

  for (Iterator iter = particlemap.begin(); iter != particlemap.end(); ++iter) {
    delete iter->second;
  }
  particlemap.clear();

  for (VtxIterator iter = vtxmap.begin(); iter != vtxmap.end(); ++iter) {
    delete iter->second;
  }
  vtxmap.clear();

  particle_embed_flags.clear();
  vertex_embed_flags.clear();
  
  return;
}

void PHG4TruthInfoContainer::identify(ostream& os) const {

  cout << "---particlemap--------------------------" << endl;
  for (ConstIterator iter = particlemap.begin(); iter != particlemap.end(); ++iter) {
    cout << "particle id " <<  iter->first << endl;
    (iter->second)->identify();
  }

  cout << "---vtxmap-------------------------------" << endl;
  for (ConstVtxIterator vter = vtxmap.begin(); vter != vtxmap.end(); ++vter) {
    cout << "vtx id: " << vter ->first << endl;
    (vter ->second)->identify();
  }
   
  cout << "---list of embeded track flags-------------------" << endl;
  for (std::map<int,int>::const_iterator eter = particle_embed_flags.begin();
       eter != particle_embed_flags.end();
       ++eter) {
    cout << "embeded track id: " << eter->first
	 << " flag: " << eter->second << endl;
  }
  
  cout << "---list of embeded vtx flags-------------------" << endl;
  for (std::map<int,int>::const_iterator eter = vertex_embed_flags.begin();
       eter != vertex_embed_flags.end();
       ++eter) {
    cout << "embeded vertex id: " << eter->first
	 << " flag: " << eter->second << endl;
  }
   
  return;
}

PHG4TruthInfoContainer::ConstIterator
PHG4TruthInfoContainer::AddParticle(const int trackid, PHG4Particle *newparticle) {
  
  int key = trackid;
  ConstIterator it;
  bool added = false;
  boost::tie(it,added) = particlemap.insert(std::make_pair(key,newparticle));
  if ( added ) return it;
  
  cerr << "PHG4TruthInfoContainer::AddParticle"
       << " - Attempt to add particle with existing trackid "
       << trackid <<": "<<newparticle ->get_name()<<" id "
       << newparticle->get_track_id()
       <<", p = ["<<newparticle->get_px()
       <<", "<<newparticle->get_py()
       <<", "<<newparticle->get_pz()<<"], "
       <<" parent ID "<<newparticle->get_parent_id()
       << std::endl;
  return particlemap.end();
}

PHG4Particle* PHG4TruthInfoContainer::GetParticle(const int trackid) {
  
  int key = trackid;
  Iterator it = particlemap.find(key);
  if ( it != particlemap.end() ) return it->second;
  return NULL;
}

PHG4Particle* PHG4TruthInfoContainer::GetPrimaryParticle(const int trackid) {
  
  if (trackid <= 0) return NULL;
  Iterator it = particlemap.find(trackid);
  if ( it != particlemap.end() ) return it->second;
  return NULL;
}

PHG4VtxPoint* PHG4TruthInfoContainer::GetVtx(const int vtxid) {
  
  int key = vtxid;
  VtxIterator it = vtxmap.find(key);
  if ( it != vtxmap.end() ) return it->second;
  return NULL;
}

PHG4VtxPoint* PHG4TruthInfoContainer::GetPrimaryVtx(const int vtxid) {

  if (vtxid <= 0) return NULL;
  VtxIterator it = vtxmap.find(vtxid);
  if ( it != vtxmap.end() ) return it->second;
  return NULL;
}

PHG4TruthInfoContainer::ConstVtxIterator
PHG4TruthInfoContainer::AddVertex(const int id, PHG4VtxPoint *newvtx) {

  int key = id;
  ConstVtxIterator it;
  bool added = false;

  if (vtxmap.find(id) != vtxmap.end()) {
    cout << "trying to add existing vtx " << id 
	 << " vtx pos: " << endl;
    (vtxmap.find(id)->second)->identify();
    identify();
  }
  
  boost::tie(it,added) = vtxmap.insert(std::make_pair(key,newvtx));
  if ( added ) {
    newvtx->set_id(key);
    return it;
  }

  cerr << "PHG4TruthInfoContainer::AddVertex"
       << " - Attempt to add vertex with existing id " << id << std::endl;
  return vtxmap.end();
}

int PHG4TruthInfoContainer::maxtrkindex() const {
  
  int key = 0;
  if (!particlemap.empty()) key = particlemap.rbegin()->first;
  if (key < 0) key = 0;
  return key;
}

int PHG4TruthInfoContainer::mintrkindex() const {
  
  int key = 0;
  if (!particlemap.empty()) key = particlemap.begin()->first;
  if (key > 0) key = 0;
  return key;
}

int PHG4TruthInfoContainer::maxvtxindex() const {
  
  int key = 0;
  if (!vtxmap.empty()) key = vtxmap.rbegin()->first;
  if (key < 0) key = 0;
  return key;
}

int PHG4TruthInfoContainer::minvtxindex() const {
  
  int key = 0;
  if (!vtxmap.empty()) key = vtxmap.begin()->first;
  if (key > 0) key = 0;
  return key;
}

void PHG4TruthInfoContainer::delete_particle(Iterator piter) {
  
  delete piter->second;
  particlemap.erase(piter);
  return;
}

void PHG4TruthInfoContainer::delete_vtx(VtxIterator viter) {
  
  delete viter->second;
  vtxmap.erase(viter);
  return;
}

int PHG4TruthInfoContainer::isEmbeded(const int trackid) const {

  std::map<int,int>::const_iterator iter = particle_embed_flags.find(trackid);
  if (iter == particle_embed_flags.end()) {
    return 0;
  }
  
  return iter->second;
}

int PHG4TruthInfoContainer::isEmbededVtx(const int vtxid) const {

  std::map<int,int>::const_iterator iter = vertex_embed_flags.find(vtxid);
  if (iter == vertex_embed_flags.end()) {
    return 0;
  }
  
  return iter->second;
}
