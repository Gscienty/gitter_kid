import React from 'react';
import ProjectTemplate from './_project';
import { Table, Icon, Card } from 'antd';
import { connect } from 'react-redux';

class Page extends ProjectTemplate {
    state = { path: '', prefixURI: '' }

    entryTreeInitialize() {
        let pathItems = window.location.hash.substring(1).split('/tree');
        let path = '/';
        if (pathItems.length === 2) {
            path = pathItems[1];
        }

        if (path !== this.state.path) {
            this.setState({ path }, () => this.freshTree(path));
        }
    }

    componentDidMount() {
        this.entryTreeInitialize();
    }

    componentWillReceiveProps(props) {
        this.entryTreeInitialize();
    }

    freshTree(path) {
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

    treeItemOnClick(record) {
        if (record.type === 'Tree') {
            this.props.history.push(this.state.prefixURI + this.state.path + record.name + '/');
            this.freshTree(this.state.path + record.name + '/');
        }
    }

    renderPath() {
        let pathItems = this.state.path.substring(0, this.state.path.length - 1).split('/');
        pathItems[0] = {
            name: '',
            path: ''
        }
        for (let i = 1; i < pathItems.length; i++) {
            pathItems[i] = {
                name: pathItems[i],
                path: pathItems[i - 1].path + '/' + pathItems[i]
            };
        }

        pathItems[0].name = this.props.match.params.repositoryName;
        pathItems[0].path = '';

        return pathItems.map((item, index) => <span style={{ fontSize: 21 }}>
            { index === 0 ? '' : '/' }
            <a onClick={() => {
                this.props.history.push(this.state.prefixURI + item.path + '/');
                this.freshTree(item.path + '/');
            }}
            style={{ margin: '0 4px' }}>{ item.name }</a>
        </span>);
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
        
        return <Card
            title={ <span>{ this.renderPath() }</span> }
            bodyStyle={{ padding: 0 }}>
            <Table
                columns={ this.columns }
                dataSource={ this.props.tree.content.map(e => ({ ...e, key: e.name })) }
                showHeader={ false }
                pagination={ false }
            />
        </Card>;
    }
}

export default connect(
    ({ tree }) => ({
        entryUnit: 'repositories', projectEntryUnit: 'code',
        tree
    })
)(Page);