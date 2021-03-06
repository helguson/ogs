/*!
  \file NodePartitionedMesh.h
  \author Wenqing Wang
  \date   2014.06
  \brief  Definition of mesh class for partitioned mesh (by node) for parallel computing within the
          framework of domain decomposition (DDC).

  \copyright
  Copyright (c) 2012-2015, OpenGeoSys Community (http://www.opengeosys.org)
             Distributed under a Modified BSD License.
               See accompanying file LICENSE.txt or
               http://www.opengeosys.org/project/license

*/

#ifndef NODE_PARTITIONED_MESH_H_
#define NODE_PARTITIONED_MESH_H_

#include <vector>
#include <string>

#include "Mesh.h"

namespace MeshLib
{
class Node;
class Element;

/// A subdomain mesh.
class NodePartitionedMesh : public Mesh
{
    public:
        /*!
            \brief Constructor
            \param name          Name assigned to the mesh.
            \param nodes         Vector for nodes, which storage looks like:
                                 ||--active base nodes--|--ghost base nodes--|
                                  --active extra nodes--|--ghost extra nodes--||
                                 (extra nodes: nodes for high order interpolations)
            \param glb_node_ids  Global IDs of nodes of a partition.
            \param elements      Vector for elements. Ghost elements are stored
                                 after regular (non-ghost) elements.
            \param n_nghost_elem Number of non-ghost elements, or the start ID of
                                 the entry of ghost element in the element vector.
            \param n_global_base_nodes Number of the base nodes of the global mesh.
            \param n_global_nodes      Number of all nodes of the global mesh.
            \param n_base_nodes        Number of the base nodes.
            \param n_active_base_nodes Number of the active base nodes.
            \param n_active_nodes      Number of all active nodes.
        */
        NodePartitionedMesh(const std::string &name,
                            const std::vector<Node*> &nodes,
                            const std::vector<std::size_t> &glb_node_ids,
                            const std::vector<Element*> &elements,
                            const std::size_t n_nghost_elem,
                            const std::size_t n_global_base_nodes,
                            const std::size_t n_global_nodes,
                            const std::size_t n_base_nodes,
                            const std::size_t n_active_base_nodes,
                            const std::size_t n_active_nodes)
            : Mesh(name, nodes, elements, n_base_nodes),
              _global_node_ids(glb_node_ids), _n_nghost_elem(n_nghost_elem),
              _n_global_base_nodes(n_global_base_nodes),
              _n_global_nodes(n_global_nodes),
              _n_active_base_nodes(n_active_base_nodes),
              _n_active_nodes(n_active_nodes)
        {
        }

        /// Get the number of nodes of the global mesh for linear elements.
        std::size_t getNGlobalBaseNodes() const
        {
            return _n_global_base_nodes;
        }

        /// Get the number of all nodes of the global mesh.
        std::size_t getNGlobalNodes() const
        {
            return _n_global_nodes;
        }

        /// Get the number of the active nodes of the partition for linear elements.
        std::size_t getNActiveBaseNodes() const
        {
            return _n_active_base_nodes;
        }

        /// Get the number of all active nodes of the partition.
        std::size_t getNActiveNodes() const
        {
            return _n_active_nodes;
        }

        /// Check whether a node with ID of node_id is a ghost node
        bool isGhostNode(const std::size_t node_id) const
        {
            if(node_id < _n_active_base_nodes)
                return true;
            else if(node_id >= _n_base_nodes && node_id < getLargestActiveNodeID() )
                return true;
            else
                return false;
        }

        /// Get the largest ID of active nodes for higher order elements in a partition.
        std::size_t getLargestActiveNodeID() const
        {
            return _n_base_nodes + _n_active_nodes - _n_active_base_nodes;
        }

        /// Get the number of non-ghost elements, or the start entry ID of ghost elements in element vector.
        std::size_t getNNonGhostElements() const
        {
            return _n_nghost_elem;
        }

    private:
        /// Global IDs of nodes of a partition
        std::vector<std::size_t> _global_node_ids;

        /// Number of non-ghost elements, or the ID of the start entry of ghost elements in _elements vector.
        std::size_t _n_nghost_elem;

        /// Number of the nodes of the global mesh linear interpolations.
        std::size_t _n_global_base_nodes;

        /// Number of all nodes of the global mesh.
        std::size_t _n_global_nodes;

        /// Number of the active nodes for linear interpolations
        std::size_t _n_active_base_nodes;

        /// Number of the all active nodes.
        std::size_t _n_active_nodes;
};

}   // namespace MeshLib

#endif // NODE_PARTITIONED_MESH_H_
