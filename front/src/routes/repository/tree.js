import React from 'react';
import ProjectTemplate from './_project';
import { Table, Icon } from 'antd';
import { connect } from 'react-redux';

class Page extends ProjectTemplate {
    state = { path: '', prefixURI: '' }

    freshTree(path) {
        if (path === undefined) {
            path = '';
            let prePaths = this.props.match.url.split('/tree');
            if (prePaths.length === 2) {
                path += prePaths[1];
            }
            else {
                path = '/'
            }

            this.setState({ path, prefixURI: prePaths[0] });
        }
        else { 
            this.setState({ path });
        }
        console.log(path);
        this.props.dispatch({
            type: 'tree/getTree',
            payload: {
                repositoriesName: this.props.match.params.repositoriesName,
                repositoryName: this.props.match.params.repositoryName,
                branchName: this.props.match.params.branchName,
                path
            }
        });
    }

    componentDidMount() {
        this.freshTree();
    }

    treeItemOnClick(record) {
        if (record.type === 'Tree') {
            this.props.history.push(this.state.prefixURI + '/tree' + this.state.path + record.name + '/');
            this.freshTree(this.state.path + record.name + '/');
        }
    }

    columns = [
        {
            key: 'fileName',
            render: (text, record) => <a onClick={ () => this.treeItemOnClick(record) }>
                <span>{ record.type === 'Blob' ? <Icon type="file-text" /> : <Icon type="folder" /> }</span>
                <span style={{ marginLeft: 8 }}>{ record.name }</span>
            </a>
        }
    ]

    unit() {

        return <div>
            <span>{ this.state.path }</span>
            <Table
                columns={ this.columns }
                dataSource={ this.props.tree.content.map(e => ({ ...e, key: e.name })) }
                showHeader={ false }
                pagination={ false }
            />
        </div>;
    }
}

export default connect(
    ({ tree }) => ({
        entryUnit: 'repositories', projectEntryUnit: 'code',
        tree
    })
)(Page);